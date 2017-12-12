__kernel void pixel(
                    __global int *img,
                    int width,
                    int height)
{
    float x0, y0, x, y, xtemp;
    int color, iter, max_gray = 255;
    
    // namest double loop
    int i = get_global_id(0) / width,
    j = get_global_id(0) % width;
    
    // stevilo iteracij
    int max_iteration = 1000;
    
    // zacetna vrednost
    x0 = (float)j / width * (float)3.5f - (float)2.5f;
    y0 = (float)i / height * (float)2.0f - (float)1.0f;
    x = 0;
    y = 0;
    iter = 0;
    while ((x*x + y*y <= 4) && (iter < max_iteration))
    {
        xtemp = x*x - y*y + x0;
        y = 2 * x*y + y0;
        x = xtemp;
        iter++;
    }
    color = (int)(iter / (float)max_iteration * (float)max_gray);
    img[get_global_id(0)] = max_gray - color;
}
