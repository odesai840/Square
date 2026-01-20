#ifndef PHYSICS_H
#define PHYSICS_H

#include "Math/Math.h"
#include "PhysicsTypes.h"
#include "Renderer/Entity.h"
#include <box2d/box2d.h>
#include <vector>
#include <unordered_map>

namespace SquareCore{

class Physics {
public:
    Physics();
    ~Physics();
    
    void Initialize();
    void Shutdown();
    
    void UpdatePhysics(std::vector<Entity>& entities, float fixedDeltaTime);
    void UpdateCollisions(std::vector<Entity>& entities);
    
    void SetGravity(float gravity);
    float GetGravity() const;
    Vec2 GetGravityVector() const;
    
    void CreateBody(Entity& entity);
    void DestroyBody(Entity& entity);
    void SyncBodyToEntity(Entity& entity);
    void SyncEntityToBody(Entity& entity);
    
    void SetColliderShape(Entity& entity, const ColliderShapeData& shapeData);
    void SetColliderBox(Entity& entity, float halfWidth, float halfHeight);
    void SetColliderCircle(Entity& entity, float radius, Vec2 center = Vec2::zero());
    void SetColliderCapsule(Entity& entity, Vec2 center1, Vec2 center2, float radius);
    void SetColliderPolygon(Entity& entity, std::vector<Vec2>& vertices);
    
    void ApplyForce(Entity& entity, const Vec2& force);
    void ApplyImpulse(Entity& entity, const Vec2& impulse);
    void SetVelocity(Entity& entity, const Vec2& velocity);
    
    void SetMass(Entity& entity, float mass);
    void SetDrag(Entity& entity, float drag);
    void SetFixedRotation(Entity& entity, bool fixed);

private:
    b2WorldId worldId;
    
    float gravityAmount = -981.0f;
    
    std::unordered_map<uint32_t, std::vector<CollisionInfo>> collisionMap;
    std::unordered_map<int64_t, uint32_t> shapeToEntityMap;

    void ProcessContactEvents(std::vector<Entity>& entities);
    void ProcessSensorEvents();
    
    int ComputeCollisionSide(const b2Vec2& normal) const;
    uint32_t GetEntityFromShape(b2ShapeId shapeId) const;
    void RegisterShape(b2ShapeId& shapeId, uint32_t entityId);
    void UnregisterShape(b2ShapeId& shapeId);
    
    b2ShapeId CreateShapeForBody(b2BodyId bodyId, Entity& entity);
    b2BodyType GetBodyType(const Entity& entity) const;
    
    Vec2 ToMeters(const Vec2& val);
    Vec2 ToCentimeters(const Vec2& val);
    float ToMeters(float val);
    float ToCentimeters(float val);
};

}

#endif
