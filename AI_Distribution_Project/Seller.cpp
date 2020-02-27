#include "Seller.h"

Seller::Seller()
{
}

Seller::Seller(float max, float spd, float travel)
{

	maxStock = max;
	salesPerDay = spd;
	travelTime = travel;
	stock = maxStock / 2;
}

float Seller::GetPercentageFull()
{
	return ((stock / maxStock) * 100.0f);
}
