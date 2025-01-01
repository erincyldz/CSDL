#include <CircleObject.h>
#include <GameObject.h>  // Include here to access GameObject definition, fuck you C
#include <GameObjectHelper.h>

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

void border_collision_detection(void* self, float delta_time)
{
    GameObject* obj = (GameObject*)self;
    if (obj->type == CIRCLE)
    {
        CircleObject* circle = (CircleObject*)obj;
        if (circle->base.position.x - circle->radius < 0 ||
            circle->base.position.x + circle->radius > WINDOW_WIDTH)
        {
            circle->base.velocity.x *= -1;
        }
        if (circle->base.position.y - circle->radius < 0 ||
            circle->base.position.y + circle->radius > WINDOW_HEIGHT)
        {
            circle->base.velocity.y *= -1;
        }
    }
    else if (obj->type == RECTANGLE)
    {
        // Rect* rect = (Rect*)obj;
        // if (rect->base.position.x < 0 || rect->base.position.x > WINDOW_WIDTH)
        // {
        //     rect->base.position.x = fmaxf(0, fminf(rect->base.position.x, WINDOW_WIDTH));
        // }
        // if (rect->base.position.y < 0 || rect->base.position.y > WINDOW_HEIGHT)
        // {
        //     rect->base.position.y = fmaxf(0, fminf(rect->base.position.y, WINDOW_HEIGHT));
        // }
    }
}

int circle_circle_collision(CircleObject* c1, CircleObject* c2)
{
    float dx = c1->base.position.x - c2->base.position.x;
    float dy = c1->base.position.y - c2->base.position.y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance <= (c1->radius + c2->radius);
}

// // int circle_rect_collision(CircleObject* circle, Rect* rect)
// // {
// //     float nearest_x = fmaxf(rect->base.position.x,
// //                             fminf(circle->base.position.x, rect->base.position.x +
// rect->width));
// //     float nearest_y = fmaxf(rect->base.position.y,
// //                             fminf(circle->base.position.y, rect->base.position.y +
// //                             rect->height));

// //     float dx = circle->base.position.x - nearest_x;
// //     float dy = circle->base.position.y - nearest_y;

// //     return (dx * dx + dy * dy) <= (circle->radius * circle->radius);
// // }

// // int rect_rect_collision(Rect* r1, Rect* r2)
// // {
// //     return !(r1->base.position.x > r2->base.position.x + r2->width ||
// //              r1->base.position.x + r1->width < r2->base.position.x ||
// //              r1->base.position.y > r2->base.position.y + r2->height ||
// //              r1->base.position.y + r1->height < r2->base.position.y);
// // }

int check_collision(GameObject* obj1, GameObject* obj2)
{
    if (obj1->type == CIRCLE && obj2->type == CIRCLE)
    {
        return circle_circle_collision((CircleObject*)obj1, (CircleObject*)obj2);
    }
    else if (obj1->type == CIRCLE && obj2->type == RECTANGLE)
    {
        // return circle_rect_collision((CircleObject*)obj1, (Rect*)obj2);
    }
    else if (obj1->type == RECTANGLE && obj2->type == CIRCLE)
    {
        // return circle_rect_collision((CircleObject*)obj2, (Rect*)obj1);
    }
    else if (obj1->type == RECTANGLE && obj2->type == RECTANGLE)
    {
        // return rect_rect_collision((Rect*)obj1, (Rect*)obj2);
    }
    return 0;  // No collision by default
}
