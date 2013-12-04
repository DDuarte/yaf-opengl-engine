
    float pixelX = moveTo.X;
    float pixelY = moveTo.Y;
    int x,y;
    pixelX -= -9.45f;
    pixelY -= 7.05f;
    
    y = static_cast<int>(-(pixelY/(-2.35f) - 6.0f));
    
    if(y % 2 == 0)
        x = static_cast<int>(-(pixelX/2.7f - 7.0f));
    else
        x = static_cast<int>(-((pixelX + 1.35f)/2.7f - 7.0f) + 1.0f);
    
     if(x == 0)
        y /= 2;