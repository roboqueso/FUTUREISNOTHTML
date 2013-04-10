#include "cinder/Rand.h"
#include "ParticleField.h"
#include "Util.h"

const int ParticleField::PARAMS_BUFFER_SIZE = 10000;
const int ParticleField::MAX_SIMULTANEOUS_PARTICLES = 500;
const int ParticleField::MIN_SIMULTANEOUS_PARTICLES = 50;
const float ParticleField::HEAT_SPAWN_AMT = 0.1f;
const float ParticleField::HEAT_KILL_THRESHOLD = 0.05f;
const float ParticleField::Z_RANGE = 1000.0f;
const int ParticleField::MAX_HISTORY_LENGTH = 150;
const double ParticleField::HEAT_CHANGE_RATE = 0.04;
const float ParticleField::MIN_RADIUS = 8.0f;
const float ParticleField::MAX_RADIUS = 60.0f;
const float ParticleField::PERLIN_START = 100000.0f;
const float ParticleField::PERLIN_EVOLVE_SPEED = 0.1f;
ci::gl::Texture* ParticleField::m_ParticleTexture;

ParticleField::ParticleField() : m_Perlin(2) {
  ci::Rand::randomize();
  m_ParamsBuffer.reserve(PARAMS_BUFFER_SIZE);
  for (int i=0; i<PARAMS_BUFFER_SIZE; i++) {
    ci::Vec4f vec = Util::GenVec4(PARAMS_BUFFER_SIZE, i);
    m_ParamsBuffer.push_back(vec);
  }
  std::random_shuffle(m_ParamsBuffer.begin(), m_ParamsBuffer.end());
  m_ParamBufferIdx = 0;
  m_NumParticles = 0;
  m_ParticleTexture = NULL;
  m_PerlinLookup = PERLIN_START + ci::Rand::randFloat()*PERLIN_START;
  m_CurTimeSeconds = 0.1;
  m_LastTimeSeconds = 0;
  m_TargetNumParticles = MIN_SIMULTANEOUS_PARTICLES;
}

void ParticleField::SetTime(double timeSeconds) {
  m_PerlinLookup = PERLIN_START + static_cast<float>(timeSeconds*PERLIN_EVOLVE_SPEED);
  m_LastTimeSeconds = m_CurTimeSeconds;
  m_CurTimeSeconds = timeSeconds;
}

void ParticleField::UpdatePerformanceInfo(float fps, float targetFps) {
  if( fps < targetFps-5 ) { //approach with a tolerance of 5FPS
    m_TargetNumParticles--;
  }
  else if( fps >= targetFps ) {
    m_TargetNumParticles++;
  }

  //clamp the number of particles
  m_TargetNumParticles = std::min(std::max(MIN_SIMULTANEOUS_PARTICLES, m_TargetNumParticles), MAX_SIMULTANEOUS_PARTICLES);
}

void ParticleField::Update() {
  updateForces();
  updateParticles();
  killParticles();
  spawnParticles();
}

void ParticleField::Render() {
  glEnable(GL_TEXTURE_2D);
  m_ParticleTexture->bind();
  static ci::Color c = ci::Color(0.4f, 0.7f, 0.9f);
  static ci::Color c2 = ci::Color(0.9f, 0.6f, 0.1f);
  glPushMatrix();
  for (std::list<Particle>::iterator it = m_Particles.begin(); it != m_Particles.end(); it++) {
    const ci::Vec4f back = it->history.back();
    float temp = static_cast<float>((it->radius-MIN_RADIUS)/(MAX_RADIUS-MIN_RADIUS));
    ci::Color curColor = c*temp + c2*(1-temp);
    float radius = it->radius * back[3];
    ci::Vec3f pos(back[0], back[1], back[2]);
    scaleVec3(pos);
    glEnable(GL_TEXTURE_2D);
    renderImage(pos, radius, curColor, back[3]);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUAD_STRIP);
    float total = static_cast<float>(it->history.size());
    for (int i = static_cast<int>(it->history.size())-1; i>0; i--) {
      float per = static_cast<float>(i) / total;
      const ci::Vec4f& cur = it->history[i];
      const ci::Vec4f& last = it->history[i-1];
      ci::Vec3f curPos(cur[0], cur[1], cur[2]);
      ci::Vec3f lastPos(last[0], last[1], last[2]);
      scaleVec3(curPos);
      scaleVec3(lastPos);

      ci::Vec3f perp0 = curPos - lastPos;
      ci::Vec3f perp1 = perp0.cross(ci::Vec3f::zAxis());
      ci::Vec3f perp2 = perp0.cross(perp1);
      perp1 = perp0.cross(perp2).normalized();
      float offWidth = (it->radius * cur[3] * per * 0.07f);
      float opacityScale = 0.95f*back[3]*per;
      if (per > 0.8f) {
        float temp = (1.0f - per) / 0.2f;
        float tempScale = sqrt(temp);
        offWidth *= tempScale;
        opacityScale *= tempScale;
      }
      ci::Vec3f off = perp1 * offWidth;
      glColor4f(curColor.r, curColor.g, curColor.b, opacityScale);
      ci::gl::vertex(curPos - off);
      ci::gl::vertex(curPos + off);
    }
    glEnd();
  }
  glPopMatrix();
}

void ParticleField::Translate(int gestureID, const ci::Vec3f& diff) {
  std::map<int, TranslationForce>::iterator it = m_TranslationForces.find(gestureID);
  TranslationForce& translation = m_TranslationForces[gestureID];
  if (it == m_TranslationForces.end()) {
    translation.curStrength = 1.0;
    translation.origStrength = 1.0;
  }
  translation.movement = diff;
  translation.lastTime = m_CurTimeSeconds;
}

void ParticleField::Rotate(int gestureID, const ci::Vec2f& center, double radius, double angle) {
  std::map<int, RotationForce>::iterator it = m_RotationForces.find(gestureID);
  RotationForce& rotation = m_RotationForces[gestureID];
  if (it == m_RotationForces.end()) {
    rotation.curAngle = angle;
    rotation.origAngle = angle;
  }
  rotation.radius = radius;
  rotation.center = center;
  rotation.center[0] *= m_NormWidth;
  rotation.center[1] *= m_NormHeight;
  rotation.lastTime = m_CurTimeSeconds;
}

void ParticleField::Repel(int gestureID, const ci::Vec2f& center, double amt) {
  RepelForce& repel = m_RepelForces[gestureID];
  repel.curStrength = amt;
  repel.origStrength = amt;
  repel.center = center;
  repel.center[0] *= m_NormWidth;
  repel.center[1] *= m_NormHeight;
  repel.lastTime = m_CurTimeSeconds;
}

void ParticleField::SetWindowSize(int width, int height) {
  m_Width = width;
  m_Height = height;
  float ratio = static_cast<float>(width)/static_cast<float>(height);
  if (ratio >= 1.0f) {
    m_NormWidth = ratio;
    m_NormHeight = 1.0f;
  } else {
    m_NormWidth = 1.0f;
    m_NormHeight = 1.0f / ratio;
  }
}

void ParticleField::SetParticleTexture(ci::gl::Texture* texture) {
  m_ParticleTexture = texture;
}

void ParticleField::updateForces() {
  std::map<int, RepelForce>::iterator repelIt = m_RepelForces.begin();
  while (repelIt != m_RepelForces.end()) {
    RepelForce& repel = repelIt->second;
    repel.curStrength = repel.origStrength * (m_CurTimeSeconds - m_LastTimeSeconds) * Util::ExponentialDecay(5.0, repel.lastTime, m_CurTimeSeconds);
    if (fabs(repel.curStrength) < 0.001) {
      std::map<int, RepelForce>::iterator tempIt = repelIt;
      repelIt++;
      m_RepelForces.erase(tempIt);
    } else {
      repelIt++;
    }
  }
  std::map<int, RotationForce>::iterator rotationIt = m_RotationForces.begin();
  while (rotationIt != m_RotationForces.end()) {
    RotationForce& rotation = rotationIt->second;
    rotation.curAngle = rotation.origAngle * (m_CurTimeSeconds - m_LastTimeSeconds) * Util::ExponentialDecay(2.0, rotation.lastTime, m_CurTimeSeconds);
    rotation.rotationMatrix = ci::Matrix22f::createRotation(static_cast<float>(rotation.curAngle));
    if (fabs(rotation.curAngle) < 0.001) {
      std::map<int, RotationForce>::iterator tempIt = rotationIt;
      rotationIt++;
      m_RotationForces.erase(tempIt);
    } else {
      rotationIt++;
    }
  }
  std::map<int, TranslationForce>::iterator translationIt = m_TranslationForces.begin();
  while (translationIt != m_TranslationForces.end()) {
    TranslationForce& translation = translationIt->second;
    translation.curStrength = translation.origStrength * (m_CurTimeSeconds - m_LastTimeSeconds) * Util::ExponentialDecay(5.0, translation.lastTime, m_CurTimeSeconds);
    if (translation.curStrength < 0.0001) {
      std::map<int, TranslationForce>::iterator tempIt = translationIt;
      translationIt++;
      m_TranslationForces.erase(tempIt);
    } else {
      translationIt++;
    }
  }
}

void ParticleField::applyForces(ci::Vec3f& vec) {
  for (std::map<int, RotationForce>::iterator it = m_RotationForces.begin(); it != m_RotationForces.end(); it++) {
    ci::Vec2f rot(vec[0], vec[1]);
    ci::Vec2f& center = it->second.center;
    ci::Matrix22f& matrix = it->second.rotationMatrix;
    rot = (matrix * (rot - center)) + center;
    float dist = (rot - center).length()/static_cast<float>(it->second.radius);
    dist = std::min(1.0f, dist);
    vec[0] = (1-dist)*rot[0] + dist*vec[0];
    vec[1] = (1-dist)*rot[1] + dist*vec[1];
  }
  for (std::map<int, RepelForce>::iterator it = m_RepelForces.begin(); it != m_RepelForces.end(); it++) {
    ci::Vec2f& center = it->second.center;
    ci::Vec2f repelDir(vec[0]-center[0], vec[1]-center[1]);
    float strength = repelDir.lengthSquared();
    strength = std::max((0.2f - strength)/0.2f, 0.0f);
    float amt = static_cast<float>(it->second.curStrength);
    vec[0] = vec[0] + amt*strength*repelDir[0];
    vec[1] = vec[1] + amt*strength*repelDir[1];
  }
  for (std::map<int, TranslationForce>::iterator it = m_TranslationForces.begin(); it != m_TranslationForces.end(); it++) {
    ci::Vec3f trans = static_cast<float>(it->second.curStrength) * it->second.movement;
    vec += trans;
  }
}

void ParticleField::scaleVec3(ci::Vec3f& vec) {
  int scale = std::min(m_Width, m_Height);
  vec[0] *= scale;
  vec[1] *= scale;
}

const ci::Vec4f& ParticleField::getParamsFromBuffer() {
  const ci::Vec4f& rslt = m_ParamsBuffer[m_ParamBufferIdx];
  m_ParamBufferIdx++;
  if (m_ParamBufferIdx >= PARAMS_BUFFER_SIZE) {
    m_ParamBufferIdx = 0;
  }
  return rslt;
}

void ParticleField::killParticles() {
  std::list<Particle>::iterator it = m_Particles.begin();
  int numKilled = 0;
  while (it != m_Particles.end()) {
    const ci::Vec4f& cur = it->history.back();
    if (cur[3] < HEAT_KILL_THRESHOLD) {
      it = m_Particles.erase(it);
      numKilled++;
    } else {
      it++;
    }
  }
  m_NumParticles -= numKilled;
}

void ParticleField::updateParticles() {
  for (std::list<Particle>::iterator it = m_Particles.begin(); it != m_Particles.end(); it++) {
    ci::Vec4f curParticle = it->history.back();
    if (curParticle[3] < HEAT_KILL_THRESHOLD) {
      it->history.pop_front();
      continue;
    }
    if (inView(curParticle)) {
      curParticle[3] = std::min(static_cast<float>(curParticle[3] + HEAT_CHANGE_RATE), 1.0f);
    } else {
      curParticle[3] = std::max(static_cast<float>(curParticle[3] - HEAT_CHANGE_RATE), 0.0f);
    }
    ci::Vec3f vel;
    ci::Vec3f updatedLoc(curParticle[0], curParticle[1], curParticle[2]);
    getPerlin(updatedLoc, vel);
    updatedLoc += it->mult * vel * (m_CurTimeSeconds - m_LastTimeSeconds);
    applyForces(updatedLoc);
    curParticle[0] = updatedLoc[0];
    curParticle[1] = updatedLoc[1];
    curParticle[2] = updatedLoc[2];
    it->history.push_back(curParticle);
    if (it->history.size() > MAX_HISTORY_LENGTH) {
      it->history.pop_front();
    }
  }
}

void ParticleField::spawnParticles() {
  int numToSpawn = m_TargetNumParticles - m_NumParticles;
  for (int i=0; i<numToSpawn; i++) {
    Particle temp;
    ci::Vec4f params = getParamsFromBuffer();
    temp.mult = std::fmod(params[0] + params[1] + params[2] + params[3], 0.6f) + 0.4f;
    temp.mult *= temp.mult;
    params[0] *= m_NormWidth;
    params[1] *= m_NormHeight;
    params[2] *= (Z_RANGE/2.0f);
    temp.radius = (MAX_RADIUS-MIN_RADIUS)*params[3] + MIN_RADIUS;
    params[3] = HEAT_SPAWN_AMT;
    temp.history.push_back(params);
    m_Particles.push_back(temp);
  }
  m_NumParticles += numToSpawn;
}

void ParticleField::getPerlin(const ci::Vec3f& pos, ci::Vec3f& vec) {
  ci::Vec3f temp(0, 0, 0.005f*pos[2] + m_PerlinLookup);
  ci::Vec3f noise = m_Perlin.dfBm(temp);
  vec = noise.normalized() * 0.09f; //0.0009f;
}

bool ParticleField::inView(const ci::Vec4f& pos) {
  return pos[0] >= 0 && pos[0] <= m_NormWidth && pos[1] >= 0 && pos[1] <= m_NormHeight && pos[2] > 0 && pos[2] < Z_RANGE/2;
}

void ParticleField::renderImage(const ci::Vec3f& loc, float diam, ci::Color col, float alpha) {
  glPushMatrix();
  glTranslatef(loc.x, loc.y, loc.z);
  glScalef(diam, diam, diam);
  glColor4f(col.r, col.g, col.b, alpha);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2f(-0.5, -0.5);
  glTexCoord2f(1, 0);
  glVertex2f(0.5, -0.5);
  glTexCoord2f(1, 1);
  glVertex2f(0.5, 0.5);
  glTexCoord2f(0, 1);
  glVertex2f(-0.5, 0.5);
  glEnd();
  glPopMatrix();
}
