__kernel void pixel(
                    __global const int *input,
                    __global int *output,
                    int width,
                    int height)
{
    int Gx, Gy, tempPixel;
    
    int shared_size = 18 * 18;
    
    __local int shared[18 * 18];
    
    int x = get_global_id(0);
    int y = get_global_id(1);
    
    size_t l_x = get_local_id(0);
    size_t l_y = get_local_id(1);
    
    // velikost skupine
    const size_t group_x = get_local_size(0);
    const size_t group_y = get_local_size(1);
    
    // globalni zaporedni index
    const size_t g_idx = x + y * width;
    // lokalni zaporedni index
    const size_t l_idx = l_y * shared_size + l_x + 1; // (+1 za robove)
    
    if(x < width && y < height){ // Če imamo več niti kot je širina in višina
        shared[l_idx] = input[g_idx]; // Trenutni pixel
        
        // left
        if (l_x == 0) {
            if (x == 0)
                shared[l_idx - 1] = 0;
            else
                shared[l_idx - 1] = input[g_idx - 1];
        }
        
        // right
        if (l_x + 1 == group_x) {
            if (x + 1 == width)
                shared[l_idx + 1] = 0;
            else
                shared[l_idx + 1] = input[g_idx + 1];
        }
        
        // top
        if (l_y == 0) {
            if (y == 0)
                shared[l_idx - shared_size] = 0;
            else
                shared[l_idx - shared_size] = input[g_idx - width];
        }
        
        // bottom
        if (l_y + 1 == group_y) {
            if (y + 1 == height)
                shared[l_idx + shared_size] = 0;
            else
                shared[l_idx + shared_size] = input[g_idx + width];
        }
        
        // top left
        if (l_x == 0 && l_y == 0){
            if(x == 0 || y == 0)
                shared[l_idx - 1 * shared_size - 1] = 0;
            else
                shared[l_idx - 1 * shared_size - 1] = input[g_idx - 1 * width - 1];
        }
        
        // bottom left
        if (l_x == 0 && l_y + 1 == group_y){
            if(x == 0 || y + 1 == height)
                shared[l_idx + 1 * shared_size - 1] = 0;
            else
                shared[l_idx + 1 * shared_size - 1] = input[g_idx + 1 * width - 1];
        }
        
        // top right
        if (l_x + 1 == group_x && l_y == 0){
            if(x + 1 == width || y == 0)
                shared[l_idx - 1 * shared_size + 1] = 0;
            else
                shared[l_idx - 1 * shared_size + 1] = input[g_idx - 1 * width + 1];
        }
        
        // bottom right
        if (l_x + 1 == group_x && l_y + 1 == group_y){
            if(x + 1 == width || y + 1 == height)
                shared[l_idx + 1 * shared_size + 1] = 0;
            else
                shared[l_idx + 1 * shared_size + 1] = input[g_idx + 1 * width + 1];
        }
    }
    
    barrier(CLK_LOCAL_MEM_FENCE);
    
    Gx = -shared[l_idx - shared_size] - 2 * shared[l_idx - 1] -
    shared[l_idx + 1 * shared_size - 1] + shared[l_idx - 1 * shared_size + 1] +
    2 * shared[l_idx + 1] + shared[l_idx + 1 * shared_size + 1];
    
    Gy = -shared[l_idx - 1 * shared_size - 1] - 2 * shared[l_idx - shared_size] -
    shared[l_idx - 1 * shared_size + 1] + shared[l_idx + 1 * shared_size - 1] +
    2 * shared[l_idx + 1] + shared[l_idx + 1 * shared_size + 1];
    
    tempPixel = sqrt((float)(Gx * Gx + Gy * Gy));
    
    if(x < height){
        if (tempPixel > 255)
            output[g_idx] = 255;
        else
            output[g_idx] = tempPixel;
    }
}
