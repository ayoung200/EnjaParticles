import pygame
from pygame.locals import *

from forces import *
from vector import Vec
import sph
from particle import Particle
from hash import Domain



def fromscreen(p, surface):
    #v.x
    p.y = surface.get_height() - p.y
    return p


def toscreen(p, surface, screen_scale):
    translate = Vec([0,0])
    p.x = translate.x + p.x*screen_scale
    p.y = surface.get_height() - (translate.y + p.y*screen_scale)
    return p


@timings
def draw_particles(ps):
    ps.reverse()
    i = 0
    for p in ps:
        if i == len(ps)-1:
            p.draw(True)
        else:
            p.draw()
        i += 1
    ps.reverse()


def main():
    pygame.init()
    screen = pygame.display.set_mode((800, 800))
    pygame.display.set_caption('SPH Forces')

    background = pygame.Surface(screen.get_size())
    background = background.convert()
    background.fill((250, 250, 250))

    clock = pygame.time.Clock()

    max_num = 2**12 #4096
    #max_num = 2**10 #1024
    #max_num = 2**8 #256
    #max_num = 2**7 #128
    
    dmin = Vec([0,0,0])
    dmax = Vec([5,5,5])
    domain = Domain(dmin, dmax)#, screen)
    system = sph.SPH(max_num, domain)

    #particles = sph.init_particles(5, system, domain, screen)
    particles = []
    p1 = Vec([2.5, 2.5]) * system.sim_scale
    particles += [ Particle(p1, system, [0,0,255], screen) ] 
    particles[0].lock = True

    p = Vec([2.5 + .8, 2.5]) * system.sim_scale
    particles += [ Particle(p, system, [255,0,0], screen) ] 
    p = Vec([2.5, 2.5 + .8]) * system.sim_scale
    particles += [ Particle(p, system, [255,0,0], screen) ] 
    p = Vec([2.5 - .8, 2.5]) * system.sim_scale
    particles += [ Particle(p, system, [255,0,0], screen) ] 
    p = Vec([2.5, 2.5 - .8]) * system.sim_scale
    particles += [ Particle(p, system, [255,0,0], screen) ] 
    p = Vec([2.5 + .8, 2.5 + .8]) * system.sim_scale
    particles += [ Particle(p, system, [255,0,0], screen) ] 



    print "p0.pos:", particles[0].pos


    mouse_down = False
    pause = True 
    pi = 5
    while 1:
        clock.tick(60)
        key = pygame.key.get_pressed()
        for event in pygame.event.get():
            if event.type == QUIT or key[K_ESCAPE] or key[K_q]:
                print "quit!"
                return
            elif key[K_t]:
                print timings

            elif key[K_0]:
                pi = 0
            elif key[K_1]:
                pi = 1
            elif key[K_2]:
                pi = 2
            elif key[K_3]:
                pi = 3
            elif key[K_4]:
                pi = 4
            elif key[K_5]:
                pi = 5

            elif key[K_p]:
                pause = not pause

            elif event.type == MOUSEBUTTONDOWN:
                mouse_down = True
            elif event.type == MOUSEMOTION:
                if(mouse_down):
                    v = Vec([event.pos[0], event.pos[1]])
                    v = fromscreen(v, screen)
                    particles[pi].move(v)
            elif event.type == MOUSEBUTTONUP:
                mouse_down = False

        
        screen.blit(background, (0, 0))

        #density_update(system, particles)
        if not pause:
            contact_update(system, particles)
            magnet_update(system, particles)
            #force_update(system, particles)
            collision_wall(system, domain, particles)
            euler_update(system, particles)
            #leapfrog_update(system, particles)

        draw_particles(particles)
        #print "ASDF"
        pygame.display.flip()

if __name__ == "__main__":
    main()