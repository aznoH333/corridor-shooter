#include "entityUtils.h"
#include "math.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"


Vector3 checkWorldCollision(float x, float y, float z, float width, float height, GameMap* map) {

    Vector3 collisionDirection = (Vector3){0};
    float halfWidth = width * 0.5f;
    float halfMapWidth = map->width * 0.5f;
    float halfHeight = height * 0.5;

    if (x - halfWidth < -10) {
        collisionDirection.x = -1;
    } else if (x + halfWidth > map->length) {
        collisionDirection.x = 1;
    }

    if (z - halfWidth < -halfMapWidth) {
        collisionDirection.z = -1;
    } else if (z + halfWidth > halfMapWidth) {
        collisionDirection.z = 1;
    }

    // check height
    if (y + halfHeight > map->ceilingHeight) {
        collisionDirection.y = 1;
    } else if (y - halfHeight < 0) {
        collisionDirection.y = -1;
    }

    return collisionDirection;
}



float approachNumber(float value, float target, float step) {
    if (value < target) {
        return min(value + step, target);
    }

    if (value > target) {
        return max(value - step, target);
    }

    return value;
}



static bool isBetween(float value, float minValue, float maxValue) {
    const float MOUSE_HIT_EPSILON = 0.0001f;
    return value >= minValue - MOUSE_HIT_EPSILON && value <= maxValue + MOUSE_HIT_EPSILON;
}

BoundingBox getEntityBoundingBox(Entity* entity) {
    float halfWidth = entity->width * 0.5f;
    float halfHeight = entity->height * 0.5f;

    return (BoundingBox) {
        .min = (Vector3) {entity->x - halfWidth, entity->y - halfHeight, entity->z - halfWidth},
        .max = (Vector3) {entity->x + halfWidth, entity->y + halfHeight, entity->z + halfWidth}
    };
}

Entity* getCollidingEntityByType(GameState* state, Entity* source, EntityType type) {
    BoundingBox sourceBox = getEntityBoundingBox(source);

    for (int i = 0; i < state->entities.count; ++i) {
        Entity* entity = &state->entities.values[i];

        if (entity == source || entity->type != type) {
            continue;
        }

        if (CheckCollisionBoxes(sourceBox, getEntityBoundingBox(entity))) {
            return entity;
        }
    }

    return NULL;
}

Entity* findEntityByType(GameState* state, Entity* source, EntityType type) {
    for (int i = 0; i < state->entities.count; ++i) {
        Entity* entity = &state->entities.values[i];

        if (entity == source || entity->type != type) {
            continue;
        }

        return entity;
    }

    return NULL;
}


static const float MOUSE_HIT_MAX_DISTANCE = 30.0f;

static void recordClosestHit(float distance, Vector3 position, float* closestDistance, Vector3* closestHit) {
    if (distance < 0 || distance >= *closestDistance) {
        return;
    }

    *closestDistance = distance;
    *closestHit = position;
}


static void checkMapXPlane(Ray ray, float planeX, float minY, float maxY, float minZ, float maxZ, float* closestDistance, Vector3* closestHit) {
    const float MOUSE_HIT_EPSILON = 0.0001f;

    if (fabsf(ray.direction.x) < MOUSE_HIT_EPSILON) {
        return;
    }

    float distance = (planeX - ray.position.x) / ray.direction.x;
    Vector3 hit = Vector3Add(ray.position, Vector3Scale(ray.direction, distance));

    if (isBetween(hit.y, minY, maxY) && isBetween(hit.z, minZ, maxZ)) {
        recordClosestHit(distance, hit, closestDistance, closestHit);
    }
}

static void checkMapYPlane(Ray ray, float planeY, float minX, float maxX, float minZ, float maxZ, float* closestDistance, Vector3* closestHit) {
    const float MOUSE_HIT_EPSILON = 0.0001f;

    if (fabsf(ray.direction.y) < MOUSE_HIT_EPSILON) {
        return;
    }

    float distance = (planeY - ray.position.y) / ray.direction.y;
    Vector3 hit = Vector3Add(ray.position, Vector3Scale(ray.direction, distance));

    if (isBetween(hit.x, minX, maxX) && isBetween(hit.z, minZ, maxZ)) {
        recordClosestHit(distance, hit, closestDistance, closestHit);
    }
}

static void checkMapZPlane(Ray ray, float planeZ, float minX, float maxX, float minY, float maxY, float* closestDistance, Vector3* closestHit) {
    const float MOUSE_HIT_EPSILON = 0.0001f;

    if (fabsf(ray.direction.z) < MOUSE_HIT_EPSILON) {
        return;
    }

    float distance = (planeZ - ray.position.z) / ray.direction.z;
    Vector3 hit = Vector3Add(ray.position, Vector3Scale(ray.direction, distance));

    if (isBetween(hit.x, minX, maxX) && isBetween(hit.y, minY, maxY)) {
        recordClosestHit(distance, hit, closestDistance, closestHit);
    }
}

Vector3 getMouseHit(GameState* state) {
    Ray mouseRay = getMouseRaycast();
    mouseRay.direction = Vector3Normalize(mouseRay.direction);

    float closestDistance = MOUSE_HIT_MAX_DISTANCE;
    Vector3 closestHit = Vector3Add(mouseRay.position, Vector3Scale(mouseRay.direction, MOUSE_HIT_MAX_DISTANCE));

    float minX = -10.0f;
    float maxX = state->map.length;
    float minY = 0.0f;
    float maxY = state->map.ceilingHeight;
    float halfMapWidth = state->map.width * 0.5f;
    float minZ = -halfMapWidth;
    float maxZ = halfMapWidth;

    checkMapXPlane(mouseRay, minX, minY, maxY, minZ, maxZ, &closestDistance, &closestHit);
    checkMapXPlane(mouseRay, maxX, minY, maxY, minZ, maxZ, &closestDistance, &closestHit);
    checkMapYPlane(mouseRay, minY, minX, maxX, minZ, maxZ, &closestDistance, &closestHit);
    checkMapYPlane(mouseRay, maxY, minX, maxX, minZ, maxZ, &closestDistance, &closestHit);
    checkMapZPlane(mouseRay, minZ, minX, maxX, minY, maxY, &closestDistance, &closestHit);
    checkMapZPlane(mouseRay, maxZ, minX, maxX, minY, maxY, &closestDistance, &closestHit);

    for (int i = 0; i < state->entities.count; ++i) {
        Entity* entity = &state->entities.values[i];
        RayCollision collision = GetRayCollisionBox(mouseRay, getEntityBoundingBox(entity));

        if (collision.hit) {
            recordClosestHit(collision.distance, collision.point, &closestDistance, &closestHit);
        }
    }

    return closestHit;
}
