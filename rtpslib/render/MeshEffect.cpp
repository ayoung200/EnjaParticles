/****************************************************************************************
* Real-Time Particle System - An OpenCL based Particle system developed to run on modern GPUs. Includes SPH fluid simulations.
* version 1.0, September 14th 2011
*
* Copyright (C) 2011 Ian Johnson, Andrew Young, Gordon Erlebacher, Myrna Merced, Evan Bollig
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
****************************************************************************************/



#include <GL/glew.h>
#include "MeshEffect.h"

using namespace std;
namespace rtps
{
    MeshEffect::MeshEffect(ShaderLibrary* lib, GLuint width, GLuint height):
        ParticleEffect(lib,width,height)
    {
      setupTimers();
    }
    MeshEffect::~MeshEffect(){}
    void MeshEffect::renderFluid(Mesh* mesh, GLuint cubeMap, GLuint sceneTex, Light* light)
    {
        if(!mesh)
            return;
        //glMatrixMode(GL_MODELVIEW);
        //glPushMatrix();
        //glMultMatrixf((float*)&mesh->modelMat);
        m_timers["render_mc_mesh"]->start();
        glPushAttrib(GL_ENABLE_BIT|GL_TEXTURE_BIT);
        glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
        if(mesh->colbo)
        {
			glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->colbo);
            glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,0,0);
        }
        if(mesh->hasNormals)
        {
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->normalbo);
            glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,0);
        }
        if(mesh->hasTexture)
        {
            glEnableVertexAttribArray(3);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->texCoordsbo);
            glVertexAttribPointer(3,2,GL_FLOAT,GL_FALSE,0,0);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,mesh->tex);
        }
        if(mesh->ibo)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
        glEnable(GL_TEXTURE_CUBE_MAP);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

        glUseProgram(m_shaderLibrary->shaders["renderFluidShader"].getProgram());
        glUniform1i(glGetUniformLocation(m_shaderLibrary->shaders["renderFluidShader"].getProgram(), "reflectionCubeSampler"), 0);
        //glUniformMatrix4fv(glGetUniformLocation(m_shaderLibrary->shaders["renderFluidShader"].getProgram(),"modelview"),1,false,modelview.m);
        //glUniformMatrix4fv(glGetUniformLocation(m_shaderLibrary->shaders["renderFluidShader"].getProgram(),"project"),1,false,project.m);
        //glUniformMatrix4fv(glGetUniformLocation(m_shaderLibrary->shaders["renderFluidShader"].getProgram(),"viewinvmat"),1,false,invModelview.m);
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderFluidShader"].getProgram(),"material.diffuse"),1,&mesh->material.diffuse.x);
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderFluidShader"].getProgram(),"material.specular"),1,&mesh->material.specular.x);
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderFluidShader"].getProgram(),"material.ambient"),1,&mesh->material.ambient.x);
        glUniform1fv(glGetUniformLocation(m_shaderLibrary->shaders["renderFluidShader"].getProgram(),"material.shininess"),1,&mesh->material.shininess);
        glUniform1fv(glGetUniformLocation(m_shaderLibrary->shaders["renderFluidShader"].getProgram(),"material.opacity"),1,&mesh->material.opacity);
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderFluidShader"].getProgram(),"light.diffuse"),1,&light->diffuse.x);
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderFluidShader"].getProgram(),"light.specular"),1,&light->specular.x);
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderFluidShader"].getProgram(),"light.ambient"),1,&light->ambient.x);
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderFluidShader"].getProgram(),"light.position"),1,&light->pos.x);

        if(mesh->iboSize)
            glDrawElements(GL_TRIANGLES,mesh->iboSize,GL_UNSIGNED_INT,0);
        else
        {
            glDrawArrays(GL_TRIANGLES,0,mesh->vboSize);
        }
        glUseProgram(0);
#if 0
        glDisableVertexAttribArray(0);
        if(mesh->colbo)
        {
            glDisableVertexAttribArray(1);
        }
        if(mesh->hasNormals)
        {
            glDisableVertexAttribArray(2);
        }
        if(mesh->hasTexture)
        {
            glDisableVertexAttribArray(3);
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,0);
        }
#endif
        glPopAttrib();
        glPopClientAttrib();
        //glPopMatrix();
        glFinish();
        m_timers["render_mc_mesh"]->stop();
    }
    void MeshEffect::render(Mesh* mesh, Light* light)
    {
        if(!mesh)
            return;
        m_timers["render_mesh"]->start();
        glPushAttrib(GL_ENABLE_BIT|GL_TEXTURE_BIT);
        glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
        //glMatrixMode(GL_MODELVIEW);
        //glPushMatrix();
        //glMultMatrixf((float*)&mesh->modelMat);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
        if(mesh->colbo)
        {
			glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->colbo);
            glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,0,0);
        }
        if(mesh->hasNormals)
        {
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->normalbo);
            glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,0);
        }
        if(mesh->hasTexture)
        {
            glEnableVertexAttribArray(3);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->texCoordsbo);
            glVertexAttribPointer(3,2,GL_FLOAT,GL_FALSE,0,0);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,mesh->tex);
        }
        if(mesh->ibo)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
        //TODO: Create My own matrix class to handle this. Or use boost. That way
        //I can be opengl 3+ compliant.
        //float16 modelview;
        //glGetFloatv(GL_MODELVIEW_MATRIX,modelview.m);
        //modelview.transpose();

        //float16 project;
        //glGetFloatv(GL_PROJECTION_MATRIX,project.m);
        //project.transpose();

        glUseProgram(m_shaderLibrary->shaders["renderLitShader"].getProgram());
        glUniformMatrix4fv(glGetUniformLocation(m_shaderLibrary->shaders["renderLitShader"].getProgram(),"meshMatrix"),1,GL_FALSE,mesh->modelMat.m);
        //glUniformMatrix4fv(glGetUniformLocation(m_shaderLibrary->shaders["renderLitShader"].getProgram(),"modelview"),1,false,modelview.m);
        //glUniformMatrix4fv(glGetUniformLocation(m_shaderLibrary->shaders["renderLitShader"].getProgram(),"project"),1,false,project.m);
        //mesh->material.diffuse.print("mat.diffuse");
        //mesh->material.specular.print("mat.spec");
        //mesh->material.ambient.print("mat.amb");
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderLitShader"].getProgram(),"material.diffuse"),1,&(mesh->material.diffuse.x));
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderLitShader"].getProgram(),"material.specular"),1,&(mesh->material.specular.x));
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderLitShader"].getProgram(),"material.ambient"),1,&(mesh->material.ambient.x));
        glUniform1fv(glGetUniformLocation(m_shaderLibrary->shaders["renderLitShader"].getProgram(),"material.shininess"),1,&(mesh->material.shininess));
        glUniform1fv(glGetUniformLocation(m_shaderLibrary->shaders["renderLitShader"].getProgram(),"material.opacity"),1,&(mesh->material.opacity));
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderLitShader"].getProgram(),"light.diffuse"),1,&(light->diffuse.x));
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderLitShader"].getProgram(),"light.specular"),1,&(light->specular.x));
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderLitShader"].getProgram(),"light.ambient"),1,&(light->ambient.x));
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderLitShader"].getProgram(),"light.position"),1,&(light->pos.x));

        if(mesh->iboSize)
            glDrawElements(GL_TRIANGLES,mesh->iboSize,GL_UNSIGNED_INT,0);
        else
        {
            glDrawArrays(GL_TRIANGLES,0,mesh->vboSize);
        }
        glUseProgram(0);
#if 0
        glDisableVertexAttribArray(0);
        if(mesh->colbo)
        {
            glDisableVertexAttribArray(1);
        }
        if(mesh->hasNormals)
        {
            glDisableVertexAttribArray(2);
        }
        if(mesh->hasTexture)
        {
            glDisableVertexAttribArray(3);
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,0);
        }
#endif
        glPopAttrib();
        glPopClientAttrib();
        glFinish();
        m_timers["render_mesh"]->stop();
        //glPopMatrix();
    }
    void MeshEffect::renderInstanced(Mesh* mesh, GLuint pos, GLuint quat, unsigned int size,Light* light)
    {
		if(size<=0)
			return;
        m_timers["render_mesh_instanced"]->start();
        glPushAttrib(GL_ENABLE_BIT|GL_TEXTURE_BIT);
        glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
        glEnableVertexAttribArray(0);
        glVertexAttribDivisorARB(0,0);
        //glEnableVertexAttribArray(1);
        //glVertexAttribDivisorARB(1,0);
        glEnableVertexAttribArray(2);
        glVertexAttribDivisorARB(2,1);
        glEnableVertexAttribArray(3);
        glVertexAttribDivisorARB(3,1);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
        //glBindBuffer(GL_ARRAY_BUFFER, mesh->colbo);
        //glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,0,0);
        glBindBuffer(GL_ARRAY_BUFFER, pos);
        glVertexAttribPointer(2,4,GL_FLOAT,GL_FALSE,0,0);
        glBindBuffer(GL_ARRAY_BUFFER, quat);
        glVertexAttribPointer(3,4,GL_FLOAT,GL_FALSE,0,0);
        if(mesh->hasNormals)
        {
            glEnableVertexAttribArray(4);
            glVertexAttribDivisorARB(4,0);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->normalbo);
            glVertexAttribPointer(4,3,GL_FLOAT,GL_FALSE,0,0);
        }
        if(mesh->hasTexture)
        {
            glEnableVertexAttribArray(5);
            glVertexAttribDivisorARB(5,0);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->texCoordsbo);
            glVertexAttribPointer(5,2,GL_FLOAT,GL_FALSE,0,0);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,mesh->tex);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
        //TODO: Create My own matrix class to handle this. Or use boost. That way
        //I can be opengl 3+ compliant.
        //float16 modelview;
        //glGetFloatv(GL_MODELVIEW_MATRIX,modelview.m);
        //modelview.transpose();

        //float16 project;
        //glGetFloatv(GL_PROJECTION_MATRIX,project.m);
        //project.transpose();

        glUseProgram(m_shaderLibrary->shaders["renderInstancedShader"].getProgram());
        //glUniformMatrix4fv(glGetUniformLocation(m_shaderLibrary->shaders["renderInstancedShader"].getProgram(),"modelview"),1,false,modelview.m);
        //glUniformMatrix4fv(glGetUniformLocation(m_shaderLibrary->shaders["renderInstancedShader"].getProgram(),"project"),1,false,project.m);
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderInstancedShader"].getProgram(),"material.diffuse"),1,&mesh->material.diffuse.x);
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderInstancedShader"].getProgram(),"material.specular"),1,&mesh->material.specular.x);
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderInstancedShader"].getProgram(),"material.ambient"),1,&mesh->material.ambient.x);
        glUniform1fv(glGetUniformLocation(m_shaderLibrary->shaders["renderInstancedShader"].getProgram(),"material.shininess"),1,&mesh->material.shininess);
        glUniform1fv(glGetUniformLocation(m_shaderLibrary->shaders["renderInstancedShader"].getProgram(),"material.opacity"),1,&mesh->material.opacity);
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderInstancedShader"].getProgram(),"light.diffuse"),1,&light->diffuse.x);
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderInstancedShader"].getProgram(),"light.specular"),1,&light->specular.x);
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderInstancedShader"].getProgram(),"light.ambient"),1,&light->ambient.x);
        glUniform3fv(glGetUniformLocation(m_shaderLibrary->shaders["renderInstancedShader"].getProgram(),"light.position"),1,&light->pos.x);
        glDrawElementsInstancedEXT(GL_TRIANGLES,mesh->iboSize,GL_UNSIGNED_INT,0,size);
        glUseProgram(0);
#if 0
        glDisableVertexAttribArray(0);
        glVertexAttribDivisorARB(0,0);
        //glDisableVertexAttribArray(1);
        //glVertexAttribDivisorARB(1,0);
        glDisableVertexAttribArray(2);
        glVertexAttribDivisorARB(2,0);
        glDisableVertexAttribArray(3);
        glVertexAttribDivisorARB(3,0);
        if(mesh->hasNormals)
        {
            glDisableVertexAttribArray(4);
            glVertexAttribDivisorARB(4,0);
        }
        if(mesh->hasTexture)
        {
            glDisableVertexAttribArray(5);
            glVertexAttribDivisorARB(5,0);
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,0);
        }
#endif
        glPopAttrib();
        glPopClientAttrib();
        glFinish();
        m_timers["render_mesh_instanced"]->stop();
    }
    void MeshEffect::setupTimers()
    {
        int time_offset = 5;
        m_timers["render_mesh"] = new EB::Timer("Render Mesh", time_offset);
        m_timers["render_mesh_instanced"] = new EB::Timer("Render Mesh Instanced",time_offset);
        m_timers["render_mc_mesh"] = new EB::Timer("Render Marching Cubes",time_offset);
    }
    void MeshEffect::printTimers()
    {
        cout<<"Mesh Effect Times"<<endl;
        m_timers.printAll();
        std::ostringstream oss;
        oss << "mesh_effects_timer_log";
        m_timers.writeToFile(oss.str());
    }
}
