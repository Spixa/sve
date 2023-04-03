#ifndef SVE_GAMEOBJECT_INCLUDED
#define SVE_GAMEOBJECT_INCLUDED

#include "model.hpp"
#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

namespace sve {
  struct TransformComponent {
    glm::vec3 translation{}; // position offset
    glm::vec3 scale{1.f,1.f, 1.f};
    glm::vec3 rotation{};

    glm::mat4 mat4();
    glm::mat3 normal_matrix();
  };

  // just began
  struct PointLightComponent {
    float lightIntensity = 1.0f;
  };


  class GameObject {
    public:
    using id_t = unsigned int;
    using Map = std::unordered_map<id_t, GameObject>;

    static GameObject createGameObject()
    {
      static id_t currentId = 0;
      return GameObject{currentId++};
    }
    
    static GameObject makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f)) {
      GameObject gameObj = GameObject::createGameObject();
      gameObj.color = color;
      gameObj.transform.scale.x = radius;
      gameObj.pointLight = std::make_unique<PointLightComponent>();
      gameObj.pointLight->lightIntensity = intensity;

      return gameObj;
    }

    GameObject(const GameObject&) = delete;
    GameObject &operator=(const GameObject&) = delete;
    GameObject(GameObject&&) = default;
    GameObject &operator=(GameObject&&) = default;

    id_t getId() { return m_id; }

    std::shared_ptr<Model> model{};
    glm::vec3 color{};


    TransformComponent transform;
    std::unique_ptr<PointLightComponent> pointLight = nullptr;
    private:
    GameObject(id_t objId) : m_id(objId) {}

    id_t m_id;
  };
};

#endif