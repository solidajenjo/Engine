#ifndef _BOSSSTATENOTACTIVE_H_
#define _BOSSSTATENOTACTIVE_H_

#include "BossState.h"

class BossStateNotActive :
	public BossState
{
public:
	BossStateNotActive(BossBehaviourScript* AIBoss);
	~BossStateNotActive();

	void HandleIA() override;
};

#endif // _BOSSSTATENOTACTIVE_H_