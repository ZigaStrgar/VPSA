int getPixel(int y, int x, int width, int height);

__kernel void pixel(
                    __global const int *input,
                    __global int *output,
                    int width,
                    int height)
{
    int Gx, Gy, tempPixel;
    
    int i = get_global_id(0) / height,
    j = get_global_id(0) % width;
    
    Gx = -input[getPixel(i - 1, j - 1, width, height)] - 2 * input[getPixel(i - 1, j, width, height)] -
    input[getPixel(i - 1, j + 1, width, height)] + input[getPixel(i + 1, j - 1, width, height)] +
    2 * input[getPixel(i + 1, j, width, height)] + input[getPixel(i + 1, j + 1, width, height)];
    
    Gy = -input[getPixel(i - 1, j - 1, width, height)] - 2 * input[getPixel(i, j - 1, width, height)] -
    input[getPixel(i + 1, j - 1, width, height)] + input[getPixel(i - 1, j + 1, width, height)] +
    2 * input[getPixel(i, j + 1, width, height)] + input[getPixel(i + 1, j + 1, width, height)];
    
    tempPixel = sqrt((float)(Gx * Gx + Gy * Gy));
    
    if (tempPixel > 255)
        output[i * width + j] = 255;
    else
        output[i * width + j] = tempPixel;
}

int getPixel(int y, int x, int width, int height) {
    if (x < 0 || x >= width)
        return 0;
    if (y < 0 || y >= height)
        return 0;
    return y * width + x;
}
