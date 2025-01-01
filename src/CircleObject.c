#include <CircleObject.h>

// Circle Initialization Function
void init_circle(CircleObject* circle, const char* name, float x, float y, float radius)
{
    strcpy(circle->base.name, name);  // Initialize GameObject name
    circle->base.position.x = x;
    circle->base.position.y = y;
    circle->base.type = CIRCLE;       // Set type to CIRCLE
    circle->base.draw = draw_circle;  // Assign draw function
    circle->base.update = update_circle;
    circle->base.collides_with = check_collision;
    circle->base.destroy = destroy_circle;
    circle->radius = fmaxf(
        MIN_RADIUS, fminf(radius, MAX_RADIUS));  // Clamp radius between MIN_RADIUS and MAX_RADIUS
}

// Circle Destruction Function
void destroy_circle(void* self)
{
    if (!self)
    {
        fprintf(stderr, "Attempt to destroy a NULL pointer\n");
        return;
    }
    // printf("Freeing CircleObject at %p named %s\n", self, ((CircleObject*)self)->base.name);
    free(self);  // Free dynamically allocated memory
}

// Circle Drawing Function
void draw_circle(void* self, SDL_Renderer* renderer)
{
    CircleObject* circle = (CircleObject*)self;  // Cast to Circle
    int centerX = (int)circle->base.position.x;
    int centerY = (int)circle->base.position.y;
    int radius = (int)circle->radius;

    // Bresenham's Circle Algorithm
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
    // fill the circle
    for (int i = -radius; i <= radius; i++)
    {
        for (int j = -radius; j <= radius; j++)
        {
            if (i * i + j * j <= radius * radius)
            {
                SDL_RenderDrawPoint(renderer, centerX + i, centerY + j);
            }
        }
    }
}

// Circle Update Function
void update_circle(void* self, float delta_time)
{
    // printf("Circle Update\n");
    CircleObject* circle = (CircleObject*)self;  // Cast to Circle
    circle->base.position.x += circle->base.velocity.x * delta_time;
    circle->base.position.y += circle->base.velocity.y * delta_time;
    border_collision_detection(self, delta_time);
    //  check the object_collisions with other objects by using check_collision
    for (size_t i = 0; i < game_object_count; i++)
    {
        if (game_objects[i] == self)
            continue;
        if (circle->base.collides_with(self, game_objects[i]))
        {
            printf("Collision detected between %s and %s\n", circle->base.name,
                   game_objects[i]->name);

            // Destroy both objects
            circle->base.destroy(self);
            game_objects[i]->destroy(game_objects[i]);

            // Remove game    // Remove both objects from the game_objects array
            for (size_t j = i; j < game_object_count - 1; j++)
            {
                game_objects[j] = game_objects[j + 1];
            }
            game_object_count--;

            // Remove `self` from the array
            for (size_t j = 0; j < game_object_count; j++)
            {
                if (game_objects[j] == self)
                {
                    for (size_t k = j; k < game_object_count - 1; k++)
                    {
                        game_objects[k] = game_objects[k + 1];
                    }
                    game_object_count--;
                    break;
                }
            }

            // Exit the loop as the current object (`self`) has been removed
            break;
        }
    }
}