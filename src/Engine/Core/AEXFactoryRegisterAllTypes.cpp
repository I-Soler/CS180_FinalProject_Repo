#include <AEX.h>
#include "../Animation/SpineComps.h"

namespace AEX
{
	/*
		Note: this function is completely optional. If we're to register all the game and engine component types,
		this file will end up with many many many includes and it will also be compiled all the time.
		It is best though to have each engine system and the game register its own components when initialized.
	*/
	void Factory::RegisterAllTypes() {

		//Graphics Components and Resources
		Register<Renderable>();
		Register<Camera>();
		Register<Texture>();
		Register<Model>();
		Register<Shader>();
		Register<ShaderProgram>();
		
		//Collision Component
		Register<Collider>();
		Register<RigidbodyComp>();
		
		//Transform Component
		Register<TransformComp>();
		
		//Logic Components
		Register<LogicComp>();
		Register<PlayerMovement>();
		Register<PlayerAttack>();
		//Register<DetachSoul>(); RIP
		Register<SceneHandler>();
		//Register<DetachSoul>();
		Register<Door>();
		Register<EnemyHealth>();
		Register<PressurePlate>();
		Register<PressPuzzleController>();
		Register<BreakableComp>();
		Register<InfoScreen>();
		Register<Fade>();
		Register<Button>();

		Register<BubbleComp>();
		Register<TurretComp>();
		Register<BulletComp>();

		//Audio Components and Resources
		Register<Sound>();
		Register<AudioSourceComp>();

		//States Machine
		Register<SuperStateComp>();
		Register<PlayerHorizontalState>();
		Register<PlayerIdleState>();
		Register<PlayerVerticalUpState>();
		Register<PlayerVerticalDownState>();
		Register<DummyState>();

		//Spine
		Register<SkeletonAnimComp>();
		Register<SkeletonRenderable>();
		Register<TextureAtlas>();
	}
}