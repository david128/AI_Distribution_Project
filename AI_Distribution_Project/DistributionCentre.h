#include <vector>
#pragma 


using namespace std;

//phase of lorry
enum Phase
{
	IDLE = 0,
	MOVING = 1,
	RETURNING = 2,
	ARRIVED = 3
};


//lorry data
struct Lorry
{
	float daysUntilArrive;
	float carryLimit;
	float carrying;
	Phase phase;
	int sellerID;
};



class DistributionCentre
{
private:
	float maxStock;
	float currentStock;

public:

	DistributionCentre();

	DistributionCentre(float max, float startingStock, Lorry l, int numOfLorries);
	
	vector<Lorry> lorries; //lorries

	//getters and setter
	void AddStock(float s) { currentStock += s; }
	void RemoveStock(float s) { currentStock -= s; }
	float GetCurrentStock() { return currentStock; }
	float GetMaxStock() { return maxStock; }
	



};

