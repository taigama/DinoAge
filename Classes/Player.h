// Player Interface for the three Dinosaurs

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Character.h"

//class Player;


#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))
#define CAST_SKILL(_player_pointer_, _skill_index_)  (_player_pointer_->*(_player_pointer_->castSkill[_skill_index_]))()

// base class of player (inherited of Character)
// can implement cast skill
// can handle overlaped blocks
class Player : public Character
{
public:	
	typedef void (Player::*CastSkill)(void);
	//typedef std::function<void(void)> CastSkill;
	// initilize
	CREATE_FUNC(Player);
	virtual bool init();

	// destructor
	virtual ~Player();

	// Overrides
	void onExit() override;
	
	// --- public method ---

	/* Call by HUD
	* -1 is left, 0 is stading, 1 is right*/
	void move(DIRECTION) override;

	// Call by HUD
	void jump() override;

	// function pointer
	//void( (Player::*) castSkill[2])() = {jump, getMaxHP};

	/*void aaaa()
	{
		castSkill[1] = &jump;
	}*/

	void noSkill() { cocos2d::MessageBox("No skill binded!", "CAST SKILL ERROR"); }
	CastSkill castSkill[3] = { &Player::noSkill, &Player::noSkill, &Player::noSkill };

	/* stating charge for spam skill
	*index: (2-3)
	##### obsolete #####*/
	void startCharge(int);

	void startCharge();
	void stopCharge();

	// get the max time of the cooldown
	float getCooldownMax(int index);
	// index: 1, 2, 3, 4
	float getCooldownRemain(int index);

	// Call by projectiles
	virtual void onHit(float dmg, DIRECTION dir, float force) override;
	
	
	// Callback function during the contact, but before collision is resolved
	virtual bool onContactPresolve(const cocos2d::PhysicsContact &contact) override;
	
	// variables
	bool isAttack();
	CC_SYNTHESIZE_READONLY(int, _moveDirect, MoveDirect);

	bool* isReadys;

protected:

	// movement
	cocos2d::Vec2 _vecMove;
	void startWalk();
	void endWalk();


	// physics
	void setColliderBitMask() override;
	float _forceFactor;

	float info_contentScaleFactor;




	// attacking
	bool _isAttack;
	float _timerAttack;

	virtual void endAttack();


	GameObject* _weapon;

	GameObject* _chargeFX;

	float* maxCooldowns;
	float* cooldowns;

	void updateCooldowns(float);


	// hurted
	float _timerHurt;

	/* knock back
	* dir : direction of player, knocking to
	* force : the force that the power source affect the player*/
	virtual void knock(DIRECTION dir, float force);




	// --- animation ----

	cocos2d::ActionInterval* act_idle;
	cocos2d::ActionInterval* act_walk;
	cocos2d::ActionInterval* act_jump;

	cocos2d::ActionInterval* act_attack;
	cocos2d::ActionInterval* act_attack2;

	cocos2d::ActionInterval* act_hurt;
	cocos2d::ActionInterval* act_die;



	// set camera follow this player
	//void updateCamera();


	// --- handling with blocks ---
	void handleOverlapBlock();	

	// handle moving block
	cocos2d::PhysicsBody* _movingBlockBody;



	// for set camera follow this player
	//cocos2d::Camera* _camera;
	//cocos2d::Rect info_mapRect;
	//cocos2d::Size info_visibleSize;
	cocos2d::Vec2 tmp_vec;


	bool _hasGround;
private:



	// for handling with blocks
	cocos2d::Vector<cocos2d::PhysicsBody*> m_blockBodies;

	//float tmp_overlapY;
	cocos2d::Vec2 tmp_myPos;
	cocos2d::Vec2 tmp_tarPos;
};
#endif // !_STATUS_H_
