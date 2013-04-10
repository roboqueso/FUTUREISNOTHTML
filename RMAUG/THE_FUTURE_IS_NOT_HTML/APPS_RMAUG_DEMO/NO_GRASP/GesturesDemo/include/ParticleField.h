#ifndef __ParticleField_h__
#define __ParticleField_h__

#include "cinder/Color.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"
#include "cinder/Perlin.h"
#include "cinder/Perlin.h"
#include "cinder/ImageIo.h"

#include "Util.h"

#include <vector>
#include <deque>
#include <list>
#include <map>

class ParticleField {

public:

  ParticleField();
  void SetTime(double timeSeconds);
  void UpdatePerformanceInfo(float fps, float targetFPS);
  void Update();
  void Render();
  void Translate(int gestureID, const ci::Vec3f& diff);
  void Rotate(int gestureID, const ci::Vec2f& center, double radius, double angle);
  void Repel(int gestureID, const ci::Vec2f& center, double amt);
  void SetWindowSize(int width, int height);
  void SetParticleTexture(ci::gl::Texture* texture);

private:

  static const int PARAMS_BUFFER_SIZE;
  static const int MAX_SIMULTANEOUS_PARTICLES;
  static const int MIN_SIMULTANEOUS_PARTICLES;
  static const float HEAT_SPAWN_AMT;
  static const float HEAT_KILL_THRESHOLD;
  static const float Z_RANGE;
  static const int MAX_HISTORY_LENGTH;
  static const double HEAT_CHANGE_RATE;
  static const float MIN_RADIUS;
  static const float MAX_RADIUS;
  static const float PERLIN_START;
  static const float PERLIN_EVOLVE_SPEED;

  typedef std::deque<ci::Vec4f> ParticleHistory;

  struct Particle {
    ParticleHistory history;
    float radius;
    float mult;
  };

  struct RotationForce {
    RotationForce() {
      origAngle = 0.0;
      curAngle = 0.0;
      lastTime = 0.0;
      center.x = 0.5f;
      center.y = 0.5f;
      rotationMatrix.setToIdentity();
    }
    double origAngle;
    double curAngle;
    double lastTime;
    double radius;
    ci::Vec2f center;
    ci::Matrix22f rotationMatrix;
  };

  struct RepelForce {
    RepelForce() {
      origStrength = 0.0;
      curStrength = 0.0;
      lastTime = 0.0;
      center.x = 0.5f;
      center.y = 0.5f;
    }
    double origStrength;
    double curStrength;
    double lastTime;
    ci::Vec2f center;
  };

  struct TranslationForce {
    TranslationForce() {
      origStrength = 0.0;
      curStrength = 0.0;
      lastTime = 0.0;
      movement[0] = 0.0f;
      movement[1] = 0.0f;
      movement[2] = 0.0f;
    }
    double origStrength;
    double curStrength;
    double lastTime;
    ci::Vec3f movement;
  };

  void updateForces();
  void applyForces(ci::Vec3f& vec);
  void scaleVec3(ci::Vec3f& vec);
  const ci::Vec4f& getParamsFromBuffer();
  void updateParticles();
  void killParticles();
  void spawnParticles();
  void getPerlin(const ci::Vec3f& pos, ci::Vec3f& vec);
  bool inView(const ci::Vec4f& pos);
  void renderImage(const ci::Vec3f& loc, float diam, ci::Color col, float alpha);

  double                            m_LastTimeSeconds;
  double                            m_CurTimeSeconds;
  static ci::gl::Texture*           m_ParticleTexture;
  std::list<Particle>               m_Particles;
  std::vector<ci::Vec4f>            m_ParamsBuffer;
  int                               m_Width;
  int                               m_Height;
  float                             m_NormWidth;
  float                             m_NormHeight;
  int                               m_ParamBufferIdx;
  int                               m_TargetNumParticles;
  int                               m_NumParticles;
  ci::Perlin                        m_Perlin;
  float                             m_PerlinLookup;
  std::map<int, RotationForce>      m_RotationForces;
  std::map<int, RepelForce>         m_RepelForces;
  std::map<int, TranslationForce>   m_TranslationForces;

};

#endif
