typedef struct AnimationFlipbook
{
    bool fixed_loop;
    Allocator allocator;
	Gfx_Image *image;

	int rows, columns;
	Vector2 sourceSize;
    Vector4 sourceRectangle;

    float speed;

    float currentFrame;
    float totalFrames;

} AnimationFlipbook;

AnimationFlipbook* 

create_anim_flipbook

(Gfx_Image *image, 
Vector2i gridSize,
int startRow,
int framesPerAnim, 
float speed, 
Allocator allocator)
{
    if (image == NULL) return 0;

    AnimationFlipbook *fb = alloc(allocator, sizeof(AnimationFlipbook));
    fb->allocator = allocator;

    fb->rows = startRow;
    fb->columns = framesPerAnim;

    fb->totalFrames = fb->rows * fb->columns;

    // the columns (y) are each item on the HORIZONTAL line so its divided by the WIDTH
    fb->sourceSize = v2(image->width / gridSize.y, image->height / gridSize.x);
    fb->sourceRectangle = v4(0, 0, fb->sourceSize.x,  fb->sourceSize.y);
    fb->currentFrame = 0;
    fb->speed = speed;

    return fb;
}

AnimationFlipbook* 
create_anim_flipbook_fixed(
Gfx_Image *image, 
Vector2i gridSize, 
float speed, 
Allocator allocator)
{
    if (image == NULL) return 0;

    AnimationFlipbook *fb = alloc(allocator, sizeof(AnimationFlipbook));
    fb->allocator = allocator;
    fb->fixed_loop = true;

    fb->rows = gridSize.x;
    fb->columns = gridSize.y;

    fb->totalFrames = fb->rows * fb->columns;

    // the columns (y) are each item on the HORIZONTAL line so its divided by the WIDTH
    fb->sourceSize = v2(image->width / gridSize.y, image->height / gridSize.x);
    fb->sourceRectangle = v4(0, 0, fb->sourceSize.x,  fb->sourceSize.y);
    fb->currentFrame = 0;
    fb->speed = speed;

    return fb;
}

AnimationFlipbook* 
update_anim_flipbook(AnimationFlipbook* fb, float64 delta)
{
    fb->currentFrame += (fb->speed * delta);

    int currFrame = (int)fb->currentFrame;
    if(currFrame >= fb->totalFrames)
    {
        currFrame = 0;
        fb->currentFrame = 0.0f;
    }

    int x = currFrame % fb->columns;
    int y = currFrame / fb->columns;
    //log("%.2f", flipbook->currentFrame);
    //log("%d", x);
    //log("%d", y);
 
    fb->sourceRectangle.x = x * fb->sourceSize.x;
    if(fb->fixed_loop)
        fb->sourceRectangle.y = y * fb->sourceSize.y;   
    else 
        fb->sourceRectangle.y = fb->rows * fb->sourceSize.y;
}