#ifndef DYNAMICOBJECT_H_
#define DYNAMICOBJECT_H_

/* Ogre3d Graphics*/
#include "Ogre.h"
#include <string>

/* Bullet3 Physics */
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"

using namespace Ogre;
using namespace std;

class DynamicObject {
  private:
    Vector3 meshBounding;
    SceneNode* node;
    Entity* object;
    btCollisionShape* colShape;
    btRigidBody* body;
    btDiscreteDynamicsWorld* dynamicsWorld;
    btScalar linearDamping;
    btScalar angularDamping;

  public:
    DynamicObject();
    ~DynamicObject();
    void createMesh(SceneManager* scnMgr, string path);
    void attachToNode(SceneNode* parent);
    void setScale(Vector3 sca);
    void setRotation(Vector3 axis, Radian angle);
    void setPosition(Vector3 pos);
    void setMass(float mass);
    void setNewDamping(float linear, float angular);
    void createRigidBody(float mass);
    void addToCollisionShapes(btAlignedObjectArray<btCollisionShape*> &collisionShapes);
    void addToDynamicsWorld(btDiscreteDynamicsWorld* dynamicsWorld);
    void boundingOgre();
    void update();
    btRigidBody* getBody(){ return body; };
    Vector3 getMeshBounding(){return meshBounding;};
    SceneNode* getNode(){return node;}
};
#endif
