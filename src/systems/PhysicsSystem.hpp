#pragma once

#include "EntityManager.hpp"
#include "System.hpp"
#include "components/PhysicsComponent.hpp"
#include "components/TransformComponent.hpp"

class PhysicsSystem : public kengine::System<PhysicsSystem> {
    public:
        PhysicsSystem(kengine::EntityManager & em) : System(em), _em(em) {}

    public:
        void execute() final {
			const auto frames = time.getDeltaFrames();

			const auto objects = _em.getEntities<
				kengine::PhysicsComponent, kengine::TransformComponent3f
			>();

			for (auto & [e, phys, transform] : objects) {
				_em.runTask([&] {
					updatePosition(phys, transform, frames);
				});
			}

			_em.completeTasks();
        }

        // Helpers
    private:
        static void updatePosition(const kengine::PhysicsComponent & phys, kengine::TransformComponent3f & transform, float frames) {
            auto & pos = transform.boundingBox.position;
            const auto dest = getNewPos(pos, phys.movement, phys.speed * frames);
            pos = dest;
        }

		static putils::Point3f getNewPos(const putils::Point3f & pos, const putils::Vector3f & movement, float speed) {
			return {
				pos.x + movement.x * speed,
				pos.y + movement.y * speed,
				pos.z + movement.z * speed,
			};
		}

    private:
        kengine::EntityManager & _em;
};
