/**
 * @file <argos3/plugins/simulator/physics_engines/dynamics2d/dynamics2d_cylinder_model.cpp>
 *
 * @author Carlo Pinciroli - <cpinciro@ulb.ac.be>
 */

#include "dynamics2d_cylinder_model.h"
#include "dynamics2d_gripping.h"
#include "dynamics2d_engine.h"

namespace argos {

   /****************************************/
   /****************************************/

   CDynamics2DCylinderModel::CDynamics2DCylinderModel(CDynamics2DEngine& c_engine,
                                                      CCylinderEntity& c_entity) :
      CDynamics2DStretchableObjectModel(c_engine, c_entity) {
      /* Get the radius of the entity */
      Real fRadius = c_entity.GetRadius();
      /* Create a circle object in the physics space */
      const CVector3& cPosition = GetEmbodiedEntity().GetOriginAnchor().Position;
      CRadians cXAngle, cYAngle, cZAngle;
      GetEmbodiedEntity().GetOriginAnchor().Orientation.ToEulerAngles(cZAngle, cYAngle, cXAngle);
      /*
       * Create body and shapes
       */
      cpBody* ptBody;
      if(GetEmbodiedEntity().IsMovable()) {
         /* The cylinder is movable */
         SetMass(c_entity.GetMass());
         /* Create the body */
         ptBody = cpSpaceAddBody(GetDynamics2DEngine().GetPhysicsSpace(),
                                 cpBodyNew(GetMass(),
                                           cpMomentForCircle(GetMass(),
                                                             0,
                                                             fRadius + fRadius,
                                                             cpvzero)));
         ptBody->p = cpv(cPosition.GetX(), cPosition.GetY());
         cpBodySetAngle(ptBody, cZAngle.GetValue());
         /* Create the shape */
         cpShape* ptShape = cpSpaceAddShape(GetDynamics2DEngine().GetPhysicsSpace(),
                                            cpCircleShapeNew(ptBody, fRadius, cpvzero));
         ptShape->e = 0.0; // no elasticity
         ptShape->u = 0.7; // lots contact friction to help pushing
         /* The shape is grippable */
         SetGrippable(new CDynamics2DGrippable(GetEmbodiedEntity(),
                                               ptShape));
         /* Set the body so that the default methods work as expected */
         SetBody(ptBody, c_entity.GetHeight());
         /* Friction with ground */
         SetLinearFriction(0.0f, c_engine.GetCylinderLinearFriction());
         SetAngularFriction(0.0f, c_engine.GetCylinderAngularFriction());
      }
      else {
         /* The cylinder is not movable */
         /* Create a static body */
         ptBody = cpBodyNewStatic();
         ptBody->p = cpv(cPosition.GetX(), cPosition.GetY());
         cpBodySetAngle(ptBody, cZAngle.GetValue());
         /* Create the shape */
         cpShape* ptShape = cpSpaceAddShape(
            GetDynamics2DEngine().GetPhysicsSpace(),
            cpCircleShapeNew(ptBody,
                             fRadius,
                             cpvzero));
         ptShape->e = 0.0; // No elasticity
         ptShape->u = 0.1; // Little contact friction to help sliding away
         /* This shape is normal (not grippable, not gripper) */
         ptShape->collision_type = CDynamics2DEngine::SHAPE_NORMAL;
         /* Set the body so that the default methods work as expected */
         SetBody(ptBody, c_entity.GetHeight());
      }
   }

   /****************************************/
   /****************************************/

   REGISTER_STANDARD_DYNAMICS2D_OPERATIONS_ON_ENTITY(CCylinderEntity, CDynamics2DCylinderModel);

   /****************************************/
   /****************************************/

}
