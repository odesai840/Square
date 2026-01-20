#include "Physics.h"
#include "SDL3/SDL_log.h"

namespace SquareCore 
{
    Physics::Physics()
    {
        worldId = b2_nullWorldId;
    }

    Physics::~Physics()
    {
        Shutdown();
    }

    void Physics::Initialize()
    {
        if (b2World_IsValid(worldId)) return;
        
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = b2Vec2{0.0f, ToMeters(gravityAmount)};
        worldId = b2CreateWorld(&worldDef);
    }

    void Physics::Shutdown()
    {
        if (b2World_IsValid(worldId))
        {
            b2DestroyWorld(worldId);
            worldId = b2_nullWorldId;
        }
        
        collisionMap.clear();
        shapeToEntityMap.clear();
    }

    void Physics::UpdatePhysics(std::vector<Entity>& entities, float fixedDeltaTime) {
        if (!b2World_IsValid(worldId)) {
            Initialize();
        }

        for (Entity& entity : entities) {
            if (entity.collider.type != ColliderType::NONE && entity.collider.enabled && entity.visible) {
                if (!entity.physicsHandle.isValid) {
                    CreateBody(entity);
                }
            }
            else if (entity.physicsHandle.isValid && !entity.visible) {
                DestroyBody(entity);
            }
        }
        
        for (Entity& entity : entities) {
            if (entity.physicsHandle.isValid && !entity.physApplied) {
                SyncBodyToEntity(entity);
            }
        }
        
        int subStepCount = 4;
        b2World_Step(worldId, fixedDeltaTime, subStepCount);
        
        for (Entity& entity : entities) {
            if (entity.physicsHandle.isValid && entity.physApplied) {
                SyncEntityToBody(entity);
            }
        }
        
        UpdateCollisions(entities);
    }

    void Physics::UpdateCollisions(std::vector<Entity>& entities) {
        collisionMap.clear();

        for (Entity& entity : entities) {
            entity.collider.ClearCollisions();
        }

        if (!b2World_IsValid(worldId)) return;
        
        ProcessContactEvents(entities);
        ProcessSensorEvents();
        
        for (Entity& entity : entities) {
            auto it = collisionMap.find(entity.ID);
            if (it != collisionMap.end()) {
                for (const auto& info : it->second) {
                    entity.collider.AddCollision(info.otherEntityId, info.side);
                }
            }
        }
    }

    void Physics::SetGravity(float gravity)
    {
        gravityAmount = gravity;
        if (b2World_IsValid(worldId))
        {
            b2World_SetGravity(worldId, b2Vec2{0.0f, ToMeters(gravity)});
        }
    }

    float Physics::GetGravity() const
    {
        return gravityAmount;
    }

    Vec2 Physics::GetGravityVector() const
    {
        return Vec2{0.0f, gravityAmount};
    }

    void Physics::CreateBody(Entity& entity)
    {
        if (!b2World_IsValid(worldId))
        {
            Initialize();
        }
        
        if (entity.physicsHandle.isValid)
        {
            DestroyBody(entity);
        }
        
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = GetBodyType(entity);
        bodyDef.position = b2Vec2{ToMeters(entity.position.x), ToMeters(entity.position.y)};
        bodyDef.rotation = b2MakeRot(-entity.rotation * MATH_PI / 180.0f);
        bodyDef.linearVelocity = b2Vec2{ToMeters(entity.velocity.x), ToMeters(entity.velocity.y)};
        bodyDef.linearDamping = entity.drag;
        bodyDef.motionLocks.angularZ = entity.fixedRotation;
        bodyDef.userData = reinterpret_cast<void*>(static_cast<uintptr_t>(entity.ID));
        
        entity.physicsHandle.bodyId = b2CreateBody(worldId, &bodyDef);
        entity.physicsHandle.isValid = true;
        
        entity.physicsHandle.shapeId = CreateShapeForBody(entity.physicsHandle.bodyId, entity);
    }

    void Physics::DestroyBody(Entity& entity) {
        if (!entity.physicsHandle.isValid) return;

        if (b2Shape_IsValid(entity.physicsHandle.shapeId)) {
            UnregisterShape(entity.physicsHandle.shapeId);
        }

        if (b2Body_IsValid(entity.physicsHandle.bodyId)) {
            b2DestroyBody(entity.physicsHandle.bodyId);
        }

        entity.physicsHandle.bodyId = b2_nullBodyId;
        entity.physicsHandle.shapeId = b2_nullShapeId;
        entity.physicsHandle.isValid = false;
    }

    void Physics::SyncBodyToEntity(Entity& entity)
    {
        if (!entity.physicsHandle.isValid || !b2Body_IsValid(entity.physicsHandle.bodyId)) return;
        
        b2Vec2 pos = {ToMeters(entity.position.x), ToMeters(entity.position.y)};
        b2Rot rot = b2MakeRot(-entity.rotation * MATH_PI / 180.0f);
        b2Body_SetTransform(entity.physicsHandle.bodyId, pos, rot);
        
        b2Vec2 vel = {ToMeters(entity.velocity.x), ToMeters(entity.velocity.y)};
        b2Body_SetLinearVelocity(entity.physicsHandle.bodyId, vel);
    }

    void Physics::SyncEntityToBody(Entity& entity)
    {
        if (!entity.physicsHandle.isValid || !b2Body_IsValid(entity.physicsHandle.bodyId)) return;
        
        b2Vec2 pos = b2Body_GetPosition(entity.physicsHandle.bodyId);
        entity.position.x = ToCentimeters(pos.x);
        entity.position.y = ToCentimeters(pos.y);
        
        b2Rot rot = b2Body_GetRotation(entity.physicsHandle.bodyId);
        entity.rotation = -b2Rot_GetAngle(rot) * 180.0f / MATH_PI;
        
        b2Vec2 vel = b2Body_GetLinearVelocity(entity.physicsHandle.bodyId);
        entity.velocity.x = ToCentimeters(vel.x);
        entity.velocity.y = ToCentimeters(vel.y);
    }

    void Physics::SetColliderShape(Entity& entity, const ColliderShapeData& shapeData)
    {
        entity.shapeData = shapeData;
        
        if (entity.physicsHandle.isValid)
        {
            if (b2Shape_IsValid(entity.physicsHandle.shapeId))
            {
                UnregisterShape(entity.physicsHandle.shapeId);
                b2DestroyShape(entity.physicsHandle.shapeId, true);
            }
            entity.physicsHandle.shapeId = CreateShapeForBody(entity.physicsHandle.bodyId, entity);
        }
    }

    void Physics::SetColliderBox(Entity& entity, float halfWidth, float halfHeight)
    {
        ColliderShapeData data;
        data.shape = ColliderShape::BOX;
        data.box.halfExtents = Vec2(halfWidth, halfHeight);
        SetColliderShape(entity, data);
    }

    void Physics::SetColliderCircle(Entity& entity, float radius, Vec2 center)
    {
        ColliderShapeData data;
        data.shape = ColliderShape::CIRCLE;
        data.circle.radius = radius;
        data.circle.center = center;
        SetColliderShape(entity, data);
    }

    void Physics::SetColliderCapsule(Entity& entity, Vec2 center1, Vec2 center2, float radius)
    {
        ColliderShapeData data;
        data.shape = ColliderShape::CAPSULE;
        data.capsule.center1 = center1;
        data.capsule.center2 = center2;
        data.capsule.radius = radius;
        SetColliderShape(entity, data);
    }

    void Physics::SetColliderPolygon(Entity& entity, std::vector<Vec2>& vertices)
    {
        ColliderShapeData data;
        data.shape = ColliderShape::POLYGON;
        data.polygon.vertices = vertices;
        SetColliderShape(entity, data);
    }

    void Physics::ApplyForce(Entity& entity, const Vec2& force)
    {
        if (!entity.physicsHandle.isValid || !b2Body_IsValid(entity.physicsHandle.bodyId)) return;
        
        b2Vec2 f = {ToMeters(force.x), ToMeters(force.y)};
        b2Body_ApplyForceToCenter(entity.physicsHandle.bodyId, f, true);
    }

    void Physics::ApplyImpulse(Entity& entity, const Vec2& impulse)
    {
        if (!entity.physicsHandle.isValid || !b2Body_IsValid(entity.physicsHandle.bodyId)) return;
        
        b2Vec2 imp = {ToMeters(impulse.x), ToMeters(impulse.y)};
        b2Body_ApplyLinearImpulseToCenter(entity.physicsHandle.bodyId, imp, true);
    }

    void Physics::SetVelocity(Entity& entity, const Vec2& velocity)
    {
        entity.velocity = velocity;
        if (entity.physicsHandle.isValid && b2Body_IsValid(entity.physicsHandle.bodyId))
        {
            b2Vec2 vel = {ToMeters(velocity.x), ToMeters(velocity.y)};
            b2Body_SetLinearVelocity(entity.physicsHandle.bodyId, vel);
            b2Body_SetAwake(entity.physicsHandle.bodyId, true);
        }
    }

    void Physics::SetMass(Entity& entity, float mass)
    {
        entity.mass = mass;
        
        if (entity.physicsHandle.isValid && b2Body_IsValid(entity.physicsHandle.bodyId))
        {
            b2MassData massData;
            massData.mass = mass;
            massData.center = b2Vec2{0.0f, 0.0f};
            massData.rotationalInertia = mass * 0.1f;
            b2Body_SetMassData(entity.physicsHandle.bodyId, massData);
        }
    }

    void Physics::SetDrag(Entity& entity, float drag)
    {
        entity.drag = drag;
        
        if (entity.physicsHandle.isValid && b2Body_IsValid(entity.physicsHandle.bodyId))
        {
            b2Body_SetLinearDamping(entity.physicsHandle.bodyId, drag);
        }
    }

    void Physics::SetFixedRotation(Entity& entity, bool fixed)
    {
        entity.fixedRotation = fixed;
        if (entity.physicsHandle.isValid && b2Body_IsValid(entity.physicsHandle.bodyId))
        {
            b2MotionLocks locks = b2Body_GetMotionLocks(entity.physicsHandle.bodyId);
            locks.angularZ = fixed;
            b2Body_SetMotionLocks(entity.physicsHandle.bodyId, locks);
        }
    }

    void Physics::ProcessContactEvents(std::vector<Entity>& entities) {
        for (Entity& entity : entities) {
            if (!entity.physicsHandle.isValid || !b2Body_IsValid(entity.physicsHandle.bodyId)) {
                continue;
            }

            int capacity = b2Body_GetContactCapacity(entity.physicsHandle.bodyId);
            if (capacity <= 0) continue;

            std::vector<b2ContactData> contacts(capacity);
            int count = b2Body_GetContactData(entity.physicsHandle.bodyId, contacts.data(), capacity);

            for (int i = 0; i < count; ++i) {
                const b2ContactData& contact = contacts[i];
                
                uint32_t otherEntityId = 0;
                b2Vec2 normal = contact.manifold.normal;

                if (GetEntityFromShape(contact.shapeIdA) == entity.ID) {
                    otherEntityId = GetEntityFromShape(contact.shapeIdB);
                } else {
                    otherEntityId = GetEntityFromShape(contact.shapeIdA);
                    normal = b2Vec2{ -normal.x, -normal.y };
                }

                if (otherEntityId == 0) continue;
                
                bool alreadyRecorded = false;
                auto it = collisionMap.find(entity.ID);
                if (it != collisionMap.end()) {
                    for (const auto& info : it->second) {
                        if (info.otherEntityId == otherEntityId) {
                            alreadyRecorded = true;
                            break;
                        }
                    }
                }

                if (!alreadyRecorded) {
                    int side = ComputeCollisionSide(normal);
                    Vec2 point = Vec2::zero();
                    if (contact.manifold.pointCount > 0) {
                        point = Vec2(
                            ToCentimeters(contact.manifold.points[0].point.x),
                            ToCentimeters(contact.manifold.points[0].point.y)
                        );
                    }

                    CollisionInfo info = { otherEntityId, side, Vec2(normal.x, normal.y), point };
                    collisionMap[entity.ID].push_back(info);
                }
            }
        }
    }

    void Physics::ProcessSensorEvents() {
        b2SensorEvents events = b2World_GetSensorEvents(worldId);

        for (int i = 0; i < events.beginCount; ++i) {
            const b2SensorBeginTouchEvent& event = events.beginEvents[i];

            uint32_t sensorEntity = GetEntityFromShape(event.sensorShapeId);
            uint32_t visitorEntity = GetEntityFromShape(event.visitorShapeId);

            if (sensorEntity == 0 || visitorEntity == 0) continue;

            CollisionInfo info = { visitorEntity, -1, Vec2::zero(), Vec2::zero() };
            collisionMap[sensorEntity].push_back(info);
            
            CollisionInfo reverseInfo = { sensorEntity, -1, Vec2::zero(), Vec2::zero() };
            collisionMap[visitorEntity].push_back(reverseInfo);
        }
    }

    int Physics::ComputeCollisionSide(const b2Vec2& normal) const
    {
        if (std::abs(normal.y) > std::abs(normal.x))
        {
            return normal.y > 0 ? 0 : 2;
        }
        else
        {
            return normal.x > 0 ? 1 : 3;
        }
    }

    uint32_t Physics::GetEntityFromShape(b2ShapeId shapeId) const
    {
        int64_t key = (static_cast<int64_t>(shapeId.index1) << 32) | shapeId.generation;
        auto it = shapeToEntityMap.find(key);
        if (it != shapeToEntityMap.end())
            return it->second;
        return 0;
    }

    void Physics::RegisterShape(b2ShapeId& shapeId, uint32_t entityId)
    {
        int64_t key = (static_cast<int64_t>(shapeId.index1) << 32) | shapeId.generation;
        shapeToEntityMap[key] = entityId;
    }

    void Physics::UnregisterShape(b2ShapeId& shapeId)
    {
        int64_t key = (static_cast<int64_t>(shapeId.index1) << 32) | shapeId.generation;
        shapeToEntityMap.erase(key);
    }

    b2ShapeId Physics::CreateShapeForBody(b2BodyId bodyId, Entity& entity)
    {
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        
        float area = 1.0f;
        if (entity.shapeData.shape == ColliderShape::CIRCLE)
        {
            float r = ToMeters(entity.shapeData.circle.radius);
            area = MATH_PI * r * r;
        }
        else if (entity.shapeData.shape == ColliderShape::BOX)
        {
            Vec2 half = ToMeters(entity.shapeData.box.halfExtents);
            area = 4.0f * half.x * half.y;
        }
        shapeDef.density = (area > 0.0f) ? (entity.mass / area) : 1.0f;
        
        shapeDef.material.friction = 0.3f;
        shapeDef.material.restitution = 0.0f;
        
        shapeDef.isSensor = (entity.collider.type == ColliderType::TRIGGER);
        shapeDef.enableContactEvents = true;
        shapeDef.enableSensorEvents = true;
        
        b2ShapeId shapeId = b2_nullShapeId;
        
        switch (entity.shapeData.shape)
        {
        case ColliderShape::CIRCLE:
            {
                b2Circle circle;
                circle.center = b2Vec2{
                    ToMeters(entity.shapeData.circle.center.x),
                    ToMeters(entity.shapeData.circle.center.y)
                };
                circle.radius = ToMeters(entity.shapeData.circle.radius);
                shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
                break;
            }
        case ColliderShape::CAPSULE:
            {
                b2Capsule capsule;
                capsule.center1 = b2Vec2{
                    ToMeters(entity.shapeData.capsule.center1.x),
                    ToMeters(entity.shapeData.capsule.center1.y)
                };
                capsule.center2 = b2Vec2{
                    ToMeters(entity.shapeData.capsule.center2.x),
                    ToMeters(entity.shapeData.capsule.center2.y)
                };
                capsule.radius = ToMeters(entity.shapeData.capsule.radius);
                shapeId = b2CreateCapsuleShape(bodyId, &shapeDef, &capsule);
                break;
            }
        case ColliderShape::POLYGON:
            {
                if (entity.shapeData.polygon.vertices.size() >= 3)
                {
                    std::vector<b2Vec2> verts(4);
                    for (const auto& v : entity.shapeData.polygon.vertices)
                    {
                        verts.push_back(b2Vec2{ToMeters(v.x), ToMeters(v.y)});
                    }
                    b2Hull hull = b2ComputeHull(verts.data(), static_cast<int>(verts.size()));
                    b2Polygon poly = b2MakePolygon(&hull, 0.0f);
                    shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &poly);
                }
                break;
            }
        case ColliderShape::BOX:
        default:
            {
                Vec2 halfExtents = entity.shapeData.box.halfExtents;
                if (halfExtents.x <= 0 || halfExtents.y <= 0) {
                    if (entity.isSpriteless) {
                        halfExtents.x = (entity.spritelessWidth * std::abs(entity.scale.x)) / 2.0f;
                        halfExtents.y = (entity.spritelessHeight * std::abs(entity.scale.y)) / 2.0f;
                    } else {
                        float frameWidth = entity.totalFrames > 1 ?
                                               (entity.spriteWidth / static_cast<float>(entity.totalFrames)) : entity.spriteWidth;
                        halfExtents.x = (frameWidth * std::abs(entity.scale.x)) / 2.0f;
                        halfExtents.y = (entity.spriteHeight * std::abs(entity.scale.y)) / 2.0f;
                    }
                }

                b2Polygon box = b2MakeOffsetBox(
                    ToMeters(halfExtents.x),
                    ToMeters(halfExtents.y),
                    b2Vec2{ ToMeters(entity.collider.offset.x), ToMeters(entity.collider.offset.y) },
                    b2MakeRot(0.0f)
                );
                shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);
                break;
            }
        }
        
        if (b2Shape_IsValid(shapeId)) {
            RegisterShape(shapeId, entity.ID);
        }

        return shapeId;
    }

    b2BodyType Physics::GetBodyType(const Entity& entity) const
    {
        if (!entity.physApplied)
        {
            return b2_staticBody;
        }
        return b2_dynamicBody;
    }

    Vec2 Physics::ToMeters(const Vec2& val)
    {
        return Vec2(val.x/100.0f, val.y/100.0f);
    }
    
    Vec2 Physics::ToCentimeters(const Vec2& val)
    {
        return Vec2(val.x*100.0f, val.y*100.0f);
    }
    
    float Physics::ToMeters(float val)
    {
        return val/100.0f;
    }

    float Physics::ToCentimeters(float val)
    {
        return val*100.0f;
    }
}
