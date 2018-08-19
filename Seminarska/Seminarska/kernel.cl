#define MAX 2147483647

int return_global_id(int x, int y, int width);

__kernel void calculateCumulatives(__global int *sobel, __global int *seamVector, int width, int height){
    // Globalni id
    //int global_x = get_global_id(0);
    
    // Lokalni id
    size_t local_x = get_local_id(0);
    
    // Velikost skupine
    const size_t group_size = get_local_size(0);

    // Dodatne spremenljivke
    int offsets = (int) (width - 1) / group_size + 1;
    
    int left = MAX, center = MAX, right = MAX, index, minIndex;
    
    // Izračunam kumulative
    for(int row = height - 2; row >= 0; row--){
        for(int offset = 0; offset < offsets; offset++){
            int x = offset * group_size + local_x;
            if(x < width){
                index = return_global_id(x, row + 1, width);
                
                center = sobel[index];
                
                if(x - 1 >= 0){
                    left = sobel[index - 1];
                }
                
                if(x + 1 < width){
                    right = sobel[index + 1];
                }
                
                sobel[return_global_id(x, row, width)] += min(min(left, center), right);
                
                left = right = center = MAX;
            }
            barrier(CLK_LOCAL_MEM_FENCE);
        }
    }
    
    int minValue = MAX;
    
    // Uporabim le 1 nit za delo (prvo)
    if(local_x == 0){
        // Pridobim začetek šiva
        for(int i = 0; i < width; i++){
            if(sobel[return_global_id(i, 0, width)] < minValue){
                minValue = sobel[return_global_id(i, 0, width)];
                minIndex = i;
            }
        }
        
        // Shranim index minimalne vrednosti prve vrstice
        seamVector[0] = minIndex;
        
        // Poiščem preostanek šiva
        for(int y = 1; y < height; y++){
            index = return_global_id(minIndex, y, width);
            
            center = sobel[index];
            
            if(minIndex - 1 >= 0){
                left = sobel[index - 1];
            }
            
            if(minIndex + 1 < width){
                right = sobel[index + 1];
            }
            
            minIndex += left < center ? (left < right ? -1 : 1) : (center < right ? 0 : 1);
            
            seamVector[y] = minIndex;
            
            left = right = center = MAX;
        }
    }
}

__kernel void removeSeam(__global int *original, __global int *final, __global int *seamVector, int width, int height){
    int x_final = get_global_id(0);
    int y_final = get_global_id(1);
    //y_final = 0;
    
    int x_original = x_final;
    
    int index_final;
    if(x_final < width && y_final < height){
        index_final = return_global_id(x_final, y_final, width);
        if (x_final >= seamVector[y_final]){
            x_original++;
        }
        final[index_final] = original[return_global_id(x_original, y_final, width + 1)];
    }
}

__kernel void sobelPixel(
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
    
    if (x >= width || y >= height){
        return;
    }
    
    Gx = -shared[l_idx - shared_size] - 2 * shared[l_idx - 1] -
    shared[l_idx + 1 * shared_size - 1] + shared[l_idx - 1 * shared_size + 1] +
    2 * shared[l_idx + 1] + shared[l_idx + 1 * shared_size + 1];
    
    Gy = -shared[l_idx - 1 * shared_size - 1] - 2 * shared[l_idx - shared_size] -
    shared[l_idx - 1 * shared_size + 1] + shared[l_idx + 1 * shared_size - 1] +
    2 * shared[l_idx + 1] + shared[l_idx + 1 * shared_size + 1];
    
    tempPixel = sqrt((float)(Gx * Gx + Gy * Gy));
    
    if (tempPixel > 255)
        output[g_idx] = 255;
    else
        output[g_idx] = tempPixel;
}

int return_global_id(int x, int y, int width){
    return y * width + x;
}

int getPixel(int y, int x, __global const int *input, int height, int width);

__kernel void pixel(
                    __global const int *input,
                    __global int *output,
                    int width,
                    int height)
{
    int Gx, Gy, tempPixel;
    
    //__local *int tab[3][258];
    
    int i = get_global_id(0) / width, j = get_global_id(0) % width;
    
    Gx = -getPixel(i - 1, j - 1, input, height, width) - 2 * getPixel(i - 1, j, input, height, width) -
    getPixel(i - 1, j + 1, input, height, width) + getPixel(i + 1, j - 1, input, height, width) +
    2 * getPixel(i + 1, j, input, height, width) + getPixel(i + 1, j + 1, input, height, width);
    
    Gy = -getPixel(i - 1, j - 1, input, height, width) - 2 * getPixel(i, j - 1, input, height, width) -
    getPixel(i + 1, j - 1, input, height, width) + getPixel(i - 1, j + 1, input, height, width) +
    2 * getPixel(i, j + 1, input, height, width) + getPixel(i + 1, j + 1, input, height, width);
    
    tempPixel = sqrt((float)(Gx * Gx + Gy * Gy));
    
    if(i < height){
        if (tempPixel > 255)
            output[i * width + j] = 255;
        else
            output[i * width + j] = tempPixel;
    }
}

int getPixel(int y, int x, __global const int *input, int height, int width) {
    if (x < 0 || x >= width)
        return 0;
    if (y < 0 || y >= height)
        return 0;
    return input[y * width + x];
}

