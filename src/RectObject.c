#include <RectObject.h>

// Rect Initialization Function
void init_rect(RectObject* rect, float x, float y, float width, float height)
{
    rect->base.position.x = x;
    rect->base.position.y = y;
    rect->base.type = RECTANGLE;  // Set type to RECTANGLE
    rect->base.update = update_rect;
    rect->base.collides_with = check_collision;
    rect->base.destroy = destroy_rect;
    rect->base.update_color = update_color;
    rect->width =
        fmaxf(MIN_SIZE, fminf(width, MAX_SIZE));  // Clamp width between MIN_SIZE and MAX_SIZE
    rect->height =
        fmaxf(MIN_SIZE, fminf(height, MAX_SIZE));  // Clamp height between MIN_SIZE and MAX_SIZE
}

// Rect Destruction Function
void destroy_rect(void* self)
{
    if (!self)
    {
        fprintf(stderr, "Attempt to destroy a NULL pointer\n");
        return;
    }
    // printf("Freeing RectObject at %p named %s\n", self, ((RectObject*)self)->base.name);
    free(self);  // Free dynamically allocated memory
}

// Rect Update Function
void update_rect(void* self, float delta_time)
{
    // printf("Rect Update\n");
    RectObject* rect = (RectObject*)self;  // Cast to Rect
    // create a random number
    rect->base.position.x += rect->base.velocity.x * delta_time;
    rect->base.position.y += rect->base.velocity.y * delta_time;
    border_collision_detection(self, delta_time);
    //  check the object_collisions with other objects by using check_collision
    for (size_t i = 0; i < game_object_count; i++)
    {
        if (game_objects[i] == self)
            continue;
        if (rect->base.collides_with(self, game_objects[i]))
        {
            // Destroy both objects
            rect->base.destroy(self);
            game_objects[i]->destroy(game_objects[i]);

            // Remove both objects from the game_objects array
            for (size_t j = i; j < game_object_count - 1; j++)
            {
                game_objects[j] = game_objects[j + 1];
            }
            game_object_count--;
        }
    }
    update_color(self, delta_time);
}