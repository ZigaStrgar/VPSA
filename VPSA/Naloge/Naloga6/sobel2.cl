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

