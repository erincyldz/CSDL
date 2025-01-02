#include <CircleObject.h>

// Circle Initialization Function
void init_circle(CircleObject* circle, float x, float y, float radius)
{
    circle->base.position.x = x;
    circle->base.position.y = y;
    circle->base.type = CIRCLE;  // Set type to CIRCLE
    circle->base.update = update_circle;
    circle->base.collides_with = check_collision;
    circle->base.destroy = destroy_circle;
    circle->base.update_color = update_color;
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
        // TODO: implement gravitational force calculations
        if (game_objects[i] == self)
            continue;
        if (circle->base.collides_with(self, game_objects[i]))
        {
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

    update_color(self, delta_time);
    // TODO: apply the gravitational force to the object
}
