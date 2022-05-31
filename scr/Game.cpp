/*-------------------------------------------------------------------------
Significant portions of this project are based on the Ogre Tutorials
- https://ogrecave.github.io/ogre/api/1.10/tutorials.html
Copyright (c) 2000-2013 Torus Knot Software Ltd

Manual generation of meshes from here:
- http://wiki.ogre3d.org/Generating+A+Mesh

*/

#include <exception>
#include <iostream>

#include "Game.h"


Game::Game() : ApplicationContext("OgreTutorialApp")
{
    dynamicsWorld = NULL;

}


Game::~Game()
{
    //cleanup in the reverse order of creation/initialization

    ///-----cleanup_start----
    //remove the rigidbodies from the dynamics world and delete them

    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
      btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
    	btRigidBody* body = btRigidBody::upcast(obj);

    	if (body && body->getMotionState())
    	{
    		delete body->getMotionState();
    	}

    	dynamicsWorld->removeCollisionObject(obj);
    	delete obj;
    }

    //delete collision shapes
    for (int j = 0; j < collisionShapes.size(); j++)
    {
    	btCollisionShape* shape = collisionShapes[j];
    	collisionShapes[j] = 0;
    	delete shape;
    }

    //delete dynamics world
    delete dynamicsWorld;

    //delete solver
    delete solver;

    //delete broadphase
    delete overlappingPairCache;

    //delete dispatcher
    delete dispatcher;

    delete collisionConfiguration;

    //next line is optional: it will be cleared by the destructor when the array goes out of scope
    collisionShapes.clear();
}


void Game::setup()
{
    // do not forget to call the base first
    ApplicationContext::setup();

    addInputListener(this);

    // get a pointer to the already created root
    Root* root = getRoot();
    scnMgr = root->createSceneManager();

    // register our scene with the RTSS
    RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scnMgr);

    bulletInit();
    setupTank();
    setupCamera();
    setupFloor();
    setupLights();
    setLevel();
}

void Game::setupCamera()
{

    Camera* cam = scnMgr->createCamera("myCam");

    //Setup Camera
    cam->setNearClipDistance(5);

    // Position Camera - to do this it must be attached to a scene graph and added
    // to the scene.
    SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    camNode->setPosition(200, 300, 800);
    camNode->lookAt(Vector3(0, 0, 0), Node::TransformSpace::TS_WORLD);
    camNode->attachObject(cam);

    // Setup viewport for the camera.
    Viewport* vp = getRenderWindow()->addViewport(cam);
    vp->setBackgroundColour(ColourValue(0, 0, 0));

    // link the camera and view port.
    cam->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
}

void Game::bulletInit()
{
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    collisionConfiguration = new btDefaultCollisionConfiguration();

     ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
     dispatcher = new btCollisionDispatcher(collisionConfiguration);

     ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
     overlappingPairCache = new btDbvtBroadphase();

     ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
     solver = new btSequentialImpulseConstraintSolver;

     dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

     dynamicsWorld->setGravity(btVector3(0, -10, 0));
}




bool Game::frameStarted (const Ogre::FrameEvent &evt)
{

  //Be sure to call base class - otherwise events are not polled.
  	ApplicationContext::frameStarted(evt);
    if (this->dynamicsWorld != NULL)
    {

       dynamicsWorld->stepSimulation((float)evt.timeSinceLastFrame, 10);

       // update positions of all objects
       for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
       {
           btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
           btRigidBody* body = btRigidBody::upcast(obj);
           btTransform trans;

           if (body && body->getMotionState())
           {
              body->getMotionState()->getWorldTransform(trans);

              /* https://oramind.com/ogre-bullet-a-beginners-basic-guide/ */
              void *userPointer = body->getUserPointer();

              // Player should know enough to update itself.
              if(userPointer == tank)
              {
                  // Ignore tank, he's always updated!
              }
              else //This is just to keep the other objects working.
              {
                if (userPointer)
                {

                  btQuaternion orientation = trans.getRotation();
                  Ogre::SceneNode *sceneNode = static_cast<Ogre::SceneNode *>(userPointer);
                  sceneNode->setPosition(Ogre::Vector3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()));
                  sceneNode->setOrientation(Ogre::Quaternion(orientation.getW(), orientation.getX(), orientation.getY(), orientation.getZ()));

                }
              }
            }
            else
            {
              trans = obj->getWorldTransform();
            }
       }

       tank->update();
   }
  return true;
}


bool Game::frameEnded (const Ogre::FrameEvent &evt)
{
  if (this->dynamicsWorld != NULL)
  {
      // Bullet can work with a fixed timestep
      dynamicsWorld->stepSimulation(1.f / 60.f, 10);

      // Or a variable one, however, under the hood it uses a fixed timestep
      // then interpolates between them.

       dynamicsWorld->stepSimulation((float)evt.timeSinceLastFrame, 10);
  }
  return true;
}

void Game::setupLights()
{
    // Setup Abient light
    scnMgr->setAmbientLight(ColourValue(0, 0, 0));
    scnMgr->setShadowTechnique(ShadowTechnique::SHADOWTYPE_STENCIL_MODULATIVE);

    // Add a spotlight
    Light* spotLight = scnMgr->createLight("SpotLight");

    // Configure
    spotLight->setDiffuseColour(0, 0, 1.0);
    spotLight->setSpecularColour(0, 0, 1.0);
    spotLight->setType(Light::LT_SPOTLIGHT);
    spotLight->setSpotlightRange(Degree(35), Degree(50));


    // Create a schene node for the spotlight
    SceneNode* spotLightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    spotLightNode->setDirection(-1, -1, 0);
    spotLightNode->setPosition(Vector3(200, 200, 0));

    // Add spotlight to the scene node.
    spotLightNode->attachObject(spotLight);

    // Create directional light
    Light* directionalLight = scnMgr->createLight("DirectionalLight");

    // Configure the light
    directionalLight->setType(Light::LT_DIRECTIONAL);
    directionalLight->setDiffuseColour(ColourValue(0.4, 0, 0));
    directionalLight->setSpecularColour(ColourValue(0.4, 0, 0));

    // Setup a scene node for the directional lightnode.
    SceneNode* directionalLightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    directionalLightNode->attachObject(directionalLight);
    directionalLightNode->setDirection(Vector3(0, -1, 1));

    // Create a point light
    Light* pointLight = scnMgr->createLight("PointLight");

    // Configure the light
    pointLight->setType(Light::LT_POINT);
    pointLight->setDiffuseColour(0.3, 0.3, 0.3);
    pointLight->setSpecularColour(0.3, 0.3, 0.3);

    // setup the scene node for the point light
    SceneNode* pointLightNode = scnMgr->getRootSceneNode()->createChildSceneNode();

    // Configure the light
    pointLightNode->setPosition(Vector3(0, 150, 250));

    // Add the light to the scene.
    pointLightNode->attachObject(pointLight);

}

bool Game::keyPressed(const KeyboardEvent& evt)
{
     if (evt.keysym.sym == SDLK_ESCAPE){getRoot()->queueEndRendering();}

   if((evt.keysym.sym == 'w')&&(evt.keysym.sym == 's')){}
   else if(evt.keysym.sym == 'w'){tank->setForwardAcceleration(1.0);}
   else if(evt.keysym.sym == 's'){tank->setForwardAcceleration(-1.0);}

   if((evt.keysym.sym == 'a')&&(evt.keysym.sym == 'd')){}
   else if(evt.keysym.sym == 'a'){tank->setTurningAcceleration(1.0);}
   else if(evt.keysym.sym == 'd'){tank->setTurningAcceleration(-1.0);}

   return true;
}


bool Game::mouseMoved(const MouseMotionEvent& evt)
{
	std::cout << "Got Mouse" << std::endl;
	return true;
}

void Game::setupTank()
{
    SceneNode* sceneRoot = scnMgr->getRootSceneNode();
    playerNode = sceneRoot->createChildSceneNode();

    tank = new Tank;
    tank->init(scnMgr, playerNode, collisionShapes, dynamicsWorld,Vector3(0,100,0));
    }

void Game::setupFloor(){
    floor = new Floor;
    floor->initFloor(scnMgr);
    floor->addToCollisionShapes(collisionShapes);
    floor->addToDynamicsWorld(dynamicsWorld);
}
void Game::setLevel(){
  SceneNode* sceneRoot = scnMgr->getRootSceneNode();
  level = sceneRoot->createChildSceneNode();
  rock1 = new Rock;
  rock1->init(scnMgr, level, collisionShapes, dynamicsWorld, Vector3(-300,50,250));
  rock2 = new Rock;
  rock2->init(scnMgr, level, collisionShapes, dynamicsWorld, Vector3(300,50,-250));
  rock3 = new Rock;
  rock3->init(scnMgr, level, collisionShapes, dynamicsWorld, Vector3(300,50,250));
  rock4 = new Rock;
  rock4->init(scnMgr, level, collisionShapes, dynamicsWorld, Vector3(-300,50,-250));
  rock5 = new Rock;
  rock5->init(scnMgr, level, collisionShapes, dynamicsWorld, Vector3(200,50,250));
}
