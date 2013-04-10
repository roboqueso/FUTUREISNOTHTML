#include <vector>
#include <boost/thread.hpp>

#include "cinder/app/AppBasic.h"
#include "Leap.h"

#include "Resources.h"
#include "ParticleField.h"

class GesturesDemoListener : public Leap::Listener {

public:
  virtual void onInit(const Leap::Controller& leap) {
    leap.enableGesture( Leap::Gesture::TYPE_SCREEN_TAP );
    leap.enableGesture( Leap::Gesture::TYPE_KEY_TAP );
    leap.enableGesture( Leap::Gesture::TYPE_CIRCLE );
    leap.enableGesture( Leap::Gesture::TYPE_SWIPE );
  }
};

class GesturesDemo : public ci::app::AppBasic {

public:

  ci::app::Renderer* prepareRenderer() { return new ci::app::RendererGl(ci::app::RendererGl::AA_MSAA_2); }

  void processGestures();

  void prepareSettings(Settings *settings);
  void setup();
  void update();
  void draw();

  void mouseDown(ci::app::MouseEvent event);
  void mouseUp(ci::app::MouseEvent event);
  void mouseWheel(ci::app::MouseEvent event);
  void keyDown(ci::app::KeyEvent event);

  ci::Vec3f normalizeCoords(const Leap::Vector& vec);

private:
  ParticleField field;
  bool mouseIsDown;
  ci::Vec2i lastMousePos;
  Leap::Controller controller;
  GesturesDemoListener listener;
  Leap::Frame lastFrame;
};

//Handle Leap Gesture processing.
//Trigger the corresponding effects in the particle field.
void GesturesDemo::processGestures() {
  Leap::Frame frame = controller.frame();

  if ( lastFrame == frame ) {
    return;
  }

  Leap::GestureList gestures =  lastFrame.isValid()       ?
                                frame.gestures(lastFrame) :
                                frame.gestures();

  lastFrame = frame;

  size_t numGestures = gestures.count();

  for (size_t i=0; i < numGestures; i++) {
    if (gestures[i].type() == Leap::Gesture::TYPE_SCREEN_TAP) {
      Leap::ScreenTapGesture tap = gestures[i];
      ci::Vec3f tapLoc = normalizeCoords(tap.position());
      field.Repel(tap.id(), ci::Vec2f(tapLoc.x, tapLoc.y), 3.0);
    } else if (gestures[i].type() == Leap::Gesture::TYPE_KEY_TAP) {
      Leap::KeyTapGesture tap = gestures[i];
      ci::Vec3f tapLoc = normalizeCoords(tap.position());
      field.Repel(tap.id(), ci::Vec2f(tapLoc.x, tapLoc.y), -3.0);
    } else if (gestures[i].type() == Leap::Gesture::TYPE_SWIPE) {
      Leap::SwipeGesture swipe = gestures[i];
      Leap::Vector diff = 0.004f*(swipe.position() - swipe.startPosition());
      ci::Vec3f curSwipe(diff.x, -diff.y, diff.z);
      field.Translate(swipe.id(), curSwipe);
    } else if (gestures[i].type() == Leap::Gesture::TYPE_CIRCLE) {
      Leap::CircleGesture circle = gestures[i];
      float progress = circle.progress();
      if (progress >= 1.0f) {
        ci::Vec3f center = normalizeCoords(circle.center());
        ci::Vec3f normal(circle.normal().x, circle.normal().y, circle.normal().z);
        double curAngle = 6.5;
        if (normal.z < 0) {
          curAngle *= -1;
        }
        field.Rotate(circle.id(), ci::Vec2f(center.x, center.y), circle.radius()/250, curAngle);
      }
    }
  }
}

//Inherited from Cinder - setup the windowing mode & take care of processing that needs to happen prior to window init.
void GesturesDemo::prepareSettings(Settings *settings) {
  settings->setWindowSize(1280, 800);
  settings->setFrameRate(120); //absurdly high on purpose
  settings->setFullScreen(false);
}

//Inherited from Cinder - post-graphics initalization setup steps.  Loads our particle texture & sets up the Leap controller.
void GesturesDemo::setup() {
  ci::gl::Texture* particleTexture = new ci::gl::Texture(ci::loadImage(ci::app::loadResource(RES_PARTICLE)));
  ci::gl::disableVerticalSync(); //required for higher than 60fps
  field.SetParticleTexture(particleTexture);
  mouseIsDown = false;
  lastMousePos.set(0, 0);
  controller.addListener( listener );
}

void GesturesDemo::update() {
  field.SetTime(ci::app::getElapsedSeconds());
  field.UpdatePerformanceInfo(getAverageFps(), getFrameRate());
  processGestures();
  field.SetWindowSize(getWindowWidth(), getWindowHeight());
  field.Update();

  if (mouseIsDown) {
    ci::Vec2i cur = getMousePos();
    ci::Vec3f diff(cur[0]-static_cast<float>(lastMousePos[0]), cur[1]-static_cast<float>(lastMousePos[1]), 0.0f);
    diff[0] /= static_cast<float>(getWindowWidth());
    diff[1] /= static_cast<float>(getWindowHeight());
    field.Translate(0, diff);
    lastMousePos = cur;
  }
  else {
    ci::Vec3f diff(0, 0, 0);
    field.Translate(0, diff);
  }
}

void GesturesDemo::draw() {
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  const float width = static_cast<float>(getWindowWidth());
  const float height = static_cast<float>(getWindowHeight());
  glBegin(GL_QUADS);
  glColor3f(0.1f,0.15f,0.3f);
  glVertex2f(0, 0);
  glVertex2f(width, 0);
  glColor3f(0,0,0);
  glVertex2f(width, height);
  glVertex2f(0, height);
  glEnd();

  field.Render();
}

//Mouse/Keyboard interaction support - primarily for testing
void GesturesDemo::mouseDown(ci::app::MouseEvent event) {
  lastMousePos = getMousePos();
  ci::Vec2f repelOrigin(getMousePos());

  repelOrigin[0] /= getWindowWidth();
  repelOrigin[1] /= getWindowHeight();

  field.Repel(0, repelOrigin, 3.0);
  mouseIsDown = true;
}

void GesturesDemo::mouseUp(ci::app::MouseEvent event) {
  mouseIsDown = false;
}

void GesturesDemo::mouseWheel(ci::app::MouseEvent event) {
  double angle = (event.getWheelIncrement() > 0) ? 2 : -2;
  ci::Vec2f temp(getMousePos());
  temp[0] /= getWindowWidth();
  temp[1] /= getWindowHeight();
  field.Rotate(0, temp, 0.5, angle);
}

void GesturesDemo::keyDown(ci::app::KeyEvent event) {
  char key = event.getChar();
  if (key == 27) { //ESC
    exit(0);
  }
  else if (key == 'f') {
    setFullScreen(!isFullScreen());
  }
}

//Converts from a standard leap coordinate to a cinder vector with all elements roughly in the range [0-1.0]
ci::Vec3f GesturesDemo::normalizeCoords(const Leap::Vector& vec) {
  static const float INTERACTION_CENTER_X = 0;
  static const float INTERACTION_CENTER_Y = 200;
  static const float INTERACTION_CENTER_Z = 0;
  static const float INTERACTION_SIZE = 400;

  ci::Vec3f result;
  result.x = (vec.x - INTERACTION_CENTER_X + (INTERACTION_SIZE/2)) / INTERACTION_SIZE;
  result.y = 1.0f - (vec.y - INTERACTION_CENTER_Y + (INTERACTION_SIZE/2)) / INTERACTION_SIZE;
  result.z = (vec.z - INTERACTION_CENTER_Z + (INTERACTION_SIZE/2)) / INTERACTION_SIZE;
  return result;
}

CINDER_APP_BASIC(GesturesDemo, ci::app::RendererGl)
