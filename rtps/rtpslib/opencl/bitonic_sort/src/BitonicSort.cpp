
/**
 * C++ port of NVIDIA's Bitonic Sort implementation
 */


template <class T>
Bitonic<T>::Bitonic(CL *cli,
                Buffer<T> *dstkey, Buffer<T> *dstval, 
                Buffer<T> *srckey, Buffer<T> *srcval)
{
    this->cli = cli;
    this->cl_dstkey = dstkey;
    this->cl_dstval = dstkey;
    this->cl_srckey = dstkey;
    this->cl_srcval = dstkey;
    loadKernels();
}

template <class T>
void Bitonic<T>::loadKernels()
{

    std::string path(BITONIC_CL_SOURCE_DIR);
    path += "/BitonicSort.cl";

    std::string options = "-D LOCAL_SIZE_LIMIT=512";
    cl::Program prog = cli->loadProgram(path, options);
    //k_bitonicSortLocal = Kernel(cli, prog, "bitonicSortLocal");
    k_bitonicSortLocal = Kernel(cli, path, "bitonicSortLocal");
    //k_bitonicSortLocal1 = Kernel(cli, prog, "bitonicSortLocal1");
    k_bitonicSortLocal1 = Kernel(cli, path, "bitonicSortLocal1");
    //k_bitonicMergeLocal = Kernel(cli, prog, "bitonicMergeLocal");
    k_bitonicMergeLocal = Kernel(cli, path, "bitonicMergeLocal");
    //k_bitonicMergeGlobal = Kernel(cli, prog, "bitonicMergeGlobal");
    k_bitonicMergeGlobal = Kernel(cli, path, "bitonicMergeGlobal");

    //TODO: implement this check with the C++ API    
    //if( (szBitonicSortLocal < (LOCAL_SIZE_LIMIT / 2)) || (szBitonicSortLocal1 < (LOCAL_SIZE_LIMIT / 2)) || (szBitonicMergeLocal < (LOCAL_SIZE_LIMIT / 2)) ){
            //shrLog("\nERROR !!! Minimum work-group size %u required by this application is not supported on this device.\n\n", LOCAL_SIZE_LIMIT / 2);

}

static cl_uint factorRadix2(cl_uint& log2L, cl_uint L){
    if(!L){
        log2L = 0;
        return 0;
    }else{
        for(log2L = 0; (L & 1) == 0; L >>= 1, log2L++);
        return L;
    }
}



template <class T>
int Bitonic<T>::Sort(int batch, int arrayLength, int dir)
{

    if(arrayLength < 2)
        return 0;

    //Only power-of-two array lengths are supported so far
    cl_uint log2L;
    cl_uint factorizationRemainder = factorRadix2(log2L, arrayLength);
    printf("bitonic factorization remainder: %d\n", factorizationRemainder);
    
    dir = (dir != 0);

    int localWorkSize;
    int globalWorkSize;

    if(arrayLength <= LOCAL_SIZE_LIMIT)
    {
         //Launch bitonicSortLocal
        int arg = 0;
        k_bitonicSortLocal.setArg(arg++, cl_dstkey->getDevicePtr());
        k_bitonicSortLocal.setArg(arg++, cl_dstval->getDevicePtr());
        k_bitonicSortLocal.setArg(arg++, cl_srckey->getDevicePtr());
        k_bitonicSortLocal.setArg(arg++, cl_srcval->getDevicePtr());
        k_bitonicSortLocal.setArg(arg++, arrayLength);
        k_bitonicSortLocal.setArg(arg++, dir); 

        localWorkSize  = LOCAL_SIZE_LIMIT / 2;
        globalWorkSize = batch * arrayLength / 2;
        k_bitonicSortLocal.execute(globalWorkSize, localWorkSize);
  
    }
    else
    {
        //Launch bitonicSortLocal1
        int arg = 0;
        k_bitonicSortLocal1.setArg(arg++, cl_dstkey->getDevicePtr());
        k_bitonicSortLocal1.setArg(arg++, cl_dstval->getDevicePtr());
        k_bitonicSortLocal1.setArg(arg++, cl_srckey->getDevicePtr());
        k_bitonicSortLocal1.setArg(arg++, cl_srcval->getDevicePtr());

        localWorkSize  = LOCAL_SIZE_LIMIT / 2;
        globalWorkSize = batch * arrayLength / 2;
        k_bitonicSortLocal1.execute(globalWorkSize, localWorkSize);

        for(uint size = 2 * LOCAL_SIZE_LIMIT; size <= arrayLength; size <<= 1)
        {
            for(unsigned stride = size / 2; stride > 0; stride >>= 1)
            {
                if(stride >= LOCAL_SIZE_LIMIT)
                {
                    //Launch bitonicMergeGlobal
                    arg = 0;
                    k_bitonicMergeGlobal.setArg(arg++, cl_dstkey->getDevicePtr());
                    k_bitonicMergeGlobal.setArg(arg++, cl_dstval->getDevicePtr());
                    k_bitonicMergeGlobal.setArg(arg++, cl_dstkey->getDevicePtr());
                    k_bitonicMergeGlobal.setArg(arg++, cl_dstval->getDevicePtr());
                    k_bitonicMergeGlobal.setArg(arg++, arrayLength);
                    k_bitonicMergeGlobal.setArg(arg++, stride);
                    k_bitonicMergeGlobal.setArg(arg++, dir); 

                    globalWorkSize = batch * arrayLength / 2;
                    k_bitonicMergeGlobal.execute(globalWorkSize);
                }
                else
                {
                    //Launch bitonicMergeLocal
                    arg = 0;
                    k_bitonicMergeLocal.setArg(arg++, cl_dstkey->getDevicePtr());
                    k_bitonicMergeLocal.setArg(arg++, cl_dstval->getDevicePtr());
                    k_bitonicMergeLocal.setArg(arg++, cl_dstkey->getDevicePtr());
                    k_bitonicMergeLocal.setArg(arg++, cl_dstval->getDevicePtr());
                    k_bitonicMergeLocal.setArg(arg++, stride);
                    k_bitonicMergeLocal.setArg(arg++, arrayLength);
                    k_bitonicMergeLocal.setArg(arg++, dir); 

                    localWorkSize  = LOCAL_SIZE_LIMIT / 2;
                    globalWorkSize = batch * arrayLength / 2;

                    k_bitonicMergeLocal.execute(globalWorkSize, localWorkSize);
                    break;
                }
                //printf("globalWorkSize: %d\n", globalWorkSize);
            }
        }
    }

    return localWorkSize;

    


    //scopy(num, cl_sort_output_hashes.getDevicePtr(), 
	//             cl_sort_hashes.getDevicePtr());
	//scopy(num, cl_sort_output_indices.getDevicePtr(), 
	//             cl_sort_indices.getDevicePtr());
    


}
