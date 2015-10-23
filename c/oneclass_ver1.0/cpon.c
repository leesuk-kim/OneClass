#pragma once
#include "cpon.h"
#include "Utility.h"

CPON* CPON_initialize(CPON* cpon)
{
	cpon = (CPON*)malloc(sizeof(CPON));
	cpon->learn = CPON_LEARN;
	cpon->test = CPON_TEST;
	return cpon;
}

void aaaaa = 1;