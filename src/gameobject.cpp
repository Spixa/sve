#include "gameobject.hpp"

namespace sve
{
  // Matrix corrsponds to Translate * Ry * Rx * Rz * inverse_scale
  // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
  // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
  glm::mat4 TransformComponent::mat4()
  {
      const float c3 = glm::cos(rotation.z);
      const float s3 = glm::sin(rotation.z);
      const float c2 = glm::cos(rotation.x);
      const float s2 = glm::sin(rotation.x);
      const float c1 = glm::cos(rotation.y);
      const float s1 = glm::sin(rotation.y);
      return glm::mat4{
          {
              scale.x * (c1 * c3 + s1 * s2 * s3),
              scale.x * (c2 * s3),
              scale.x * (c1 * s2 * s3 - c3 * s1),
              0.0f,
          },
          {
              scale.y * (c3 * s1 * s2 - c1 * s3),
              scale.y * (c2 * c3),
              scale.y * (c1 * c3 * s2 + s1 * s3),
              0.0f,
          },
          {
              scale.z * (c2 * s1),
              scale.z * (-s2),
              scale.z * (c1 * c2),
              0.0f,
          },
          {translation.x, translation.y, translation.z, 1.0f}};
  }

  glm::mat3 TransformComponent::normal_matrix()
  {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);

    const glm::vec3 inverse_scale = 1.f/scale;

    return glm::mat3{
      {
          inverse_scale.x * (c1 * c3 + s1 * s2 * s3),
          inverse_scale.x * (c2 * s3),
          inverse_scale.x * (c1 * s2 * s3 - c3 * s1),
      },
      {
          inverse_scale.y * (c3 * s1 * s2 - c1 * s3),
          inverse_scale.y * (c2 * c3),
          inverse_scale.y * (c1 * c3 * s2 + s1 * s3),
      },
      {
          inverse_scale.z * (c2 * s1),
          inverse_scale.z * (-s2),
          inverse_scale.z * (c1 * c2),
    }};
  }

};