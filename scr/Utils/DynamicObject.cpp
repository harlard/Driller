#include "DynamicObject.h"

DynamicObject::DynamicObject(){
  node = nullptr;
  object = nullptr;
  Vector3 meshBounding(0.0f,0.0f,0.0f);
  colShape = nullptr;
  dynamicsWorld = nullptr;
  linearDamping = 0.2f;
  angularDamping = 0.8f;
}

DynamicObject::~DynamicObject(){

}

void DynamicObject::createMesh(SceneManager* scnMgr, string path)
{
  object = scnMgr->createEntity(path);
}

void DynamicObject::attachToNode(SceneNode* parent)
{
  node = parent->createChildSceneNode();
  node->attachObject(object);
  node->setScale(1.0f,1.0f,1.0f);
  boundingOgre();
}

void DynamicObject::boundingOgre()
{
  node->_updateBounds();
  const AxisAlignedBox& b = node->_getWorldAABB();
  Vector3 temp(b.getSize());
  meshBounding = temp;
}

void DynamicObject::setScale(Vector3 sca){
  node->setScale(sca);
  boundingOgre();
}


void DynamicObject::setRotation(Vector3 axis, Radian rads){
    Quaternion quat(rads, axis);
  node->setOrientation(quat);
}

void DynamicObject::setPosition(Vector3 pos){  node->setPosition(pos);}
void DynamicObject::setNewDamping(float linear, float angular){
  linearDamping = linear;
  angularDamping = angular;
}


void DynamicObject::createRigidBody(float bodyMass)
{
  colShape = new btBoxShape(btVector3(meshBounding.x/2.0f, meshBounding.y/2.0f, meshBounding.z/2.0f));

  /// Create Dynamic Objects
  btTransform startTransform;
  startTransform.setIdentity();

  Quaternion quat2 = node->_getDerivedOrientation();
  startTransform.setRotation(btQuaternion(quat2.x, quat2.y, quat2.z, quat2.w));

  Vector3 pos = node->_getDerivedPosition();
  startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));

  btScalar mass(bodyMass);

  //rigidbody is dynamic if and only if mass is non zero, otherwise static
  bool isDynamic = (mass != 0.f);

  btVector3 localInertia(0, 0, 0);
  if (isDynamic)
  {
      // Debugging
      //std::cout << "I see the cube is dynamic" << std::endl;
      colShape->calculateLocalInertia(mass, localInertia);
  }

  //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
  btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
  body = new btRigidBody(rbInfo);

  //Set the linear and angular damping
  //I'm using this to bring the object to rest when moving.
  //An alternative would be to use friciton for the collison.
  //No good for hovering stuff though.
  body->setDamping(linearDamping,angularDamping);

  //Set the user pointer to this object.
  //body->setUserPointer((void*)this);
}

void DynamicObject::addToCollisionShapes(btAlignedObjectArray<btCollisionShape*> &collisionShapes)
{
  collisionShapes.push_back(colShape);
}

void DynamicObject::addToDynamicsWorld(btDiscreteDynamicsWorld* dynamicsWorld)
{
  this->dynamicsWorld = dynamicsWorld;
  dynamicsWorld->addRigidBody(body);
}

void DynamicObject::update()
{
  btTransform trans;

  if (body && body->getMotionState())
  {
    body->getMotionState()->getWorldTransform(trans);
    btQuaternion orientation = trans.getRotation();

    node->setPosition(Ogre::Vector3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()));
    node->setOrientation(Ogre::Quaternion(orientation.getW(), orientation.getX(), orientation.getY(), orientation.getZ()));
  }
}
