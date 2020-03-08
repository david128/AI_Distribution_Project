#include <iostream>
#include "Seller.h"
#include "DistributionCentre.h"
#include <string>
#include <ctime>
#include <fstream>
#include "fl/Headers.h"

using namespace std;
using namespace fl;


vector<Seller> sellers;
DistributionCentre dist;
int numOfSellers = 0;
int numOfLorries = 0;
vector<float> salesMissed;


float stockLow = 50.0f;
float farDist = 3.0f;
float highSPD = 10.0f;

Engine* engine = new Engine;
InputVariable* Stock = new InputVariable;
InputVariable* Distance = new InputVariable;
InputVariable* SalesPerDay = new InputVariable;
InputVariable* DeliveryOnItsWay = new InputVariable;
OutputVariable* urgency = new OutputVariable;
RuleBlock* ruleBlock = new RuleBlock;

struct DecisionData
{
	float stockPer;
	float dist;
	float spd;
	bool deliveryOnItsWay;
};

struct FuzzyDecisionData
{
	float stockPer;
	float dist;
	float spd;
	float deliveryOnItsWay;
};

void ReadInFiles()
{

	std::ifstream sellersFile("sellers.txt");
	std::ifstream lorryFile("lorries.txt");
	float sm = 0;
	string a, b, c;
	
	while(sellersFile >> a >> b >> c)
	{
		
		
		
		sellers.push_back(Seller(stof(a), stof(b), stof(c)));
		salesMissed.push_back(sm);
	}


	Lorry lorry;
	

	lorryFile >> lorry.carryLimit >> numOfLorries;
	lorry.daysUntilArrive = 0.0f;
	lorry.carrying = 0.0f;
	lorry.phase = IDLE;
	dist = DistributionCentre(500.0f, 300.0f, lorry, numOfLorries);


	numOfSellers = sellers.size();
}



void MoveStock(int sellerID, int LorryID)
{

	
	dist.lorries[LorryID].carrying = dist.lorries[LorryID].carryLimit;


	dist.lorries[LorryID].daysUntilArrive = sellers[sellerID].GetTravelTime();
	dist.lorries[LorryID].sellerID = sellerID;
	dist.lorries[LorryID].phase = MOVING;

	std::cout << ("Truck " + to_string(LorryID + 1) + " has been dispatched to shop " + to_string(dist.lorries[LorryID].sellerID + 1) + " carrying  " + to_string(dist.lorries[LorryID].carrying) + " stock and will arrive in " + to_string(dist.lorries[LorryID].daysUntilArrive) + " days\n");
}


void DeliverStock(int sellerID, int LorryID)
{
	float newStock = sellers[sellerID].GetStock() + dist.lorries[LorryID].carrying;
	if (newStock <= sellers[sellerID].GetMaxStock())
	{
		sellers[sellerID].AddStock(dist.lorries[LorryID].carrying);
		std::cout << ("Truck " + to_string(LorryID + 1) + " has delivered " + to_string(dist.lorries[LorryID].carrying ) + " to shop " + to_string(dist.lorries[LorryID].sellerID + 1) + "\n");
		dist.lorries[LorryID].carrying = 0;

	}
	else
	{
		
		std::cout << ("Truck " + to_string(LorryID + 1) + " has delivered " + to_string((sellers[sellerID].GetMaxStock() - sellers[sellerID].GetStock())) + " to shop " + to_string(dist.lorries[LorryID].sellerID + 1) + "\n");
		sellers[sellerID].SetStock(sellers[sellerID].GetMaxStock());
		dist.lorries[LorryID].carrying = 0;
	}

	
}

void Sell(int sellerID)
{
	cout << ("Shop " + to_string(sellerID+1) + " started with "  +  to_string(sellers[sellerID].GetStock()));

	if (sellers[sellerID].GetStock() >= sellers[sellerID].GetSalesPerDay())
	{
		cout << (" stock, sold " + to_string(sellers[sellerID].GetSalesPerDay()));
		sellers[sellerID].SellStock(sellers[sellerID].GetSalesPerDay());
		cout << (" stock and ended with " + to_string(sellers[sellerID].GetStock()) + "stock, missing 0 sales\n");
	}
	else
	{
		cout << (" stock, sold " + to_string(sellers[sellerID].GetStock()));
		
		int sm = sellers[sellerID].GetSalesPerDay() - sellers[sellerID].GetStock();
		salesMissed[sellerID] += sm;
		sellers[sellerID].SellStock(sellers[sellerID].GetStock());
		cout << (" stock ended with " + to_string(sellers[sellerID].GetStock()) + "stock, missing "+ to_string(sm )+" sales\n");
	}
	
}

DecisionData GetDecisionData(int sellerID)
{
	DecisionData decisionData;

	decisionData.stockPer = sellers[sellerID].GetPercentageFull();
	decisionData.dist = sellers[sellerID].GetTravelTime();
	decisionData.spd = sellers[sellerID].GetSalesPerDay();
	decisionData.deliveryOnItsWay = false;
	for (int i = 0; i < numOfLorries; i++)
	{
		if (dist.lorries[i].phase == MOVING && dist.lorries[i].sellerID == sellerID)
		{
			decisionData.deliveryOnItsWay = true;
		}
	}
	
	return decisionData;
}

FuzzyDecisionData GetFuzzyDecisionData(int sellerID)
{
	FuzzyDecisionData decisionData;
	
	decisionData.stockPer = sellers[sellerID].GetPercentageFull();
	decisionData.dist = sellers[sellerID].GetTravelTime();
	decisionData.spd = sellers[sellerID].GetSalesPerDay();
	decisionData.deliveryOnItsWay = 7;
	float num = 0;
	for (int i = 0; i < numOfLorries; i++)
	{
		
		if (dist.lorries[i].phase == MOVING && dist.lorries[i].sellerID == sellerID )
		{
			num++;
			if (dist.lorries[i].daysUntilArrive < decisionData.deliveryOnItsWay)
			{
				decisionData.deliveryOnItsWay = dist.lorries[i].daysUntilArrive;
			}
			
		}
	}

	if (num != 0)
	{
		decisionData.deliveryOnItsWay = decisionData.deliveryOnItsWay / num;
	}


	return decisionData;
}

void SetUpFuzzyLogic()
{

	//Code automatically generated with fuzzylite 6.0.


	
	engine->setName("distributionFuzzy1");
	engine->setDescription("");

	
	Stock->setName("Stock");
	Stock->setDescription("");
	Stock->setEnabled(true);
	Stock->setRange(0.000, 100.000);
	Stock->setLockValueInRange(false);
	Stock->addTerm(new Triangle("Empty", -25.000, 0.000, 15.000));
	Stock->addTerm(new Triangle("Low", 10.300, 25.600, 50.600));
	Stock->addTerm(new Triangle("Medium", 30.000, 50.000, 70.000));
	Stock->addTerm(new Triangle("WellStocked", 50.000, 75.000, 90.000));
	Stock->addTerm(new Triangle("Full", 85.000, 100.000, 125.000));
	engine->addInputVariable(Stock);

	
	Distance->setName("Distance");
	Distance->setDescription("");
	Distance->setEnabled(true);
	Distance->setRange(0.000, 6.000);
	Distance->setLockValueInRange(false);
	Distance->addTerm(new Triangle("Close", -3.000, 0.000, 3.000));
	Distance->addTerm(new Triangle("MediumDistance", 1.500, 3.000, 4.500));
	Distance->addTerm(new Triangle("Far", 3.000, 6.000, 9.000));
	engine->addInputVariable(Distance);

	
	SalesPerDay->setName("SalesPerDay");
	SalesPerDay->setDescription("");
	SalesPerDay->setEnabled(true);
	SalesPerDay->setRange(0.000, 20.000);
	SalesPerDay->setLockValueInRange(false);
	SalesPerDay->addTerm(new Triangle("Low", -8.000, 0.000, 8.000));
	SalesPerDay->addTerm(new Triangle("Moderate", 4.000, 10.000, 16.011));
	SalesPerDay->addTerm(new Triangle("High", 12.000, 20.000, 28.000));
	engine->addInputVariable(SalesPerDay);

	DeliveryOnItsWay->setName("DeliveryOnItsWay");
	DeliveryOnItsWay->setDescription("");
	DeliveryOnItsWay->setEnabled(true);
	DeliveryOnItsWay->setRange(0.000, 7.000);
	DeliveryOnItsWay->setLockValueInRange(false);
	DeliveryOnItsWay->addTerm(new Triangle("Soon", -2.500, 0.000, 2.500));
	DeliveryOnItsWay->addTerm(new Triangle("OnItsWay", 1.500, 2.500, 5.000));
	DeliveryOnItsWay->addTerm(new Triangle("NotSoon", 4.000, 5.000, 6.000));
	DeliveryOnItsWay->addTerm(new Triangle("NoDelivery", 6.000, 7.000, 9.350));
	engine->addInputVariable(DeliveryOnItsWay);

	urgency->setName("urgency");
	urgency->setDescription("");
	urgency->setEnabled(true);
	urgency->setRange(0.000, 1.000);
	urgency->setLockValueInRange(false);
	urgency->setAggregation(new Maximum);
	urgency->setDefuzzifier(new Centroid(100));
	urgency->setDefaultValue(fl::nan);
	urgency->setLockPreviousValue(false);
	urgency->addTerm(new Triangle("VeryLow", -0.250, 0.000, 0.250));
	urgency->addTerm(new Triangle("Low", 0.000, 0.250, 0.500));
	urgency->addTerm(new Triangle("Average", 0.250, 0.500, 0.750));
	urgency->addTerm(new Triangle("High", 0.500, 0.750, 1.000));
	urgency->addTerm(new Triangle("VeryHigh", 0.750, 1.000, 1.250));
	engine->addOutputVariable(urgency);

	ruleBlock->setName("");
	ruleBlock->setDescription("");
	ruleBlock->setEnabled(true);
	ruleBlock->setConjunction(new Minimum);
	ruleBlock->setDisjunction(new Maximum);
	ruleBlock->setImplication(new Minimum);
	ruleBlock->setActivation(new General);
	ruleBlock->addRule(Rule::parse("if Stock is Empty then urgency is VeryHigh", engine));
	ruleBlock->addRule(Rule::parse("if Stock is Low then urgency is High", engine));
	ruleBlock->addRule(Rule::parse("if Stock is Medium then urgency is Average", engine));
	ruleBlock->addRule(Rule::parse("if Stock is WellStocked then urgency is Low", engine));
	ruleBlock->addRule(Rule::parse("if Stock is Full then urgency is VeryLow", engine));
	ruleBlock->addRule(Rule::parse("if Distance is Close then urgency is Low", engine));
	ruleBlock->addRule(Rule::parse("if Distance is MediumDistance then urgency is Average", engine));
	ruleBlock->addRule(Rule::parse("if Distance is Far then urgency is High", engine));
	ruleBlock->addRule(Rule::parse("if SalesPerDay is Low then urgency is Low", engine));
	ruleBlock->addRule(Rule::parse("if SalesPerDay is Moderate then urgency is Average", engine));
	ruleBlock->addRule(Rule::parse("if SalesPerDay is High then urgency is High", engine));
	ruleBlock->addRule(Rule::parse("if DeliveryOnItsWay is Soon then urgency is VeryLow", engine));
	ruleBlock->addRule(Rule::parse("if DeliveryOnItsWay is OnItsWay then urgency is Low", engine));
	ruleBlock->addRule(Rule::parse("if DeliveryOnItsWay is NotSoon then urgency is Average", engine));
	ruleBlock->addRule(Rule::parse("if DeliveryOnItsWay is NoDelivery then urgency is High", engine));
	engine->addRuleBlock(ruleBlock);


	
}

void runFuzzyLogic(int lorryID)
{
	vector<float> sellerUrgency;
	for (int i = 0; i < numOfSellers; i++)
	{
		FuzzyDecisionData d = GetFuzzyDecisionData(i);

		Stock->setValue(d.stockPer);
		Distance->setValue(d.dist);
		SalesPerDay->setValue(d.spd);
		DeliveryOnItsWay->setValue(d.deliveryOnItsWay);

		engine->process();


		sellerUrgency.push_back(urgency->getValue());
		
	}
	
	int mostUrgent = 0;
	for (int i = 1; i < numOfSellers; i++)
	{
		if (sellerUrgency[mostUrgent] < sellerUrgency[i])
		{
			mostUrgent = i;
		}
	}

	MoveStock(mostUrgent, lorryID);
	
}

void RuleBased(int lorryID)
{
	vector<float> urgency = { 0 };
	//check sellers and give each a value for how urgent they need a delivery
	for (int i = 0; i < numOfSellers; i++)
	{
		float thisUrgency = 0;
		urgency.push_back(thisUrgency);
		DecisionData d = GetDecisionData(i);
		if (d.stockPer < stockLow)
		{
			urgency[i] += 3;
		}
		else
		{
			urgency[i] -= 3;
		}

		if (d.dist > farDist)
		{
			urgency[i] += 1;
		}
		else
		{
			urgency[i] -= 1;
		}

		if (d.spd > highSPD)
		{
			urgency[i] += 1;
		}
		else
		{
			urgency[i] -= 1;
		}

		if (d.deliveryOnItsWay == false)
		{
			urgency[i] += 1;
		}
		else
		{
			urgency[i] -= 1;
		}
	}

	int mostUrgent = 0;
	for (int i = 1; i < numOfSellers; i++)
	{
		if (urgency[mostUrgent] < urgency[i])
		{
			
			mostUrgent = i;
		}
	}

	MoveStock(mostUrgent, lorryID);
}

void RandomPlaceholder(int lorryID)
{
	
	MoveStock((rand() % 3),lorryID);
	
}

void CheckLorries()
{
	for (int i = 0; i < numOfLorries; i++)
	{
		if (dist.lorries[i].phase == IDLE)
		{
			//RandomPlaceholder(i);
			//RuleBased(i);
			runFuzzyLogic(i);
		}
		else if (dist.lorries[i].phase == MOVING)
		{
			if (dist.lorries[i].daysUntilArrive > 1)
			{
				dist.lorries[i].daysUntilArrive -= 1;
				std::cout << ("Truck  " + to_string(i+1) + " is on its way to shop " + to_string(dist.lorries[i].sellerID+1) + " will arrive in " + to_string(dist.lorries[i].daysUntilArrive)  +" days\n");
			}
			else
			{
				dist.lorries[i].daysUntilArrive -= 1;
				DeliverStock(dist.lorries[i].sellerID,i);
				dist.lorries[i].phase = ARRIVED;
			}
		}
		else if (dist.lorries[i].phase == ARRIVED)
		{
			dist.lorries[i].daysUntilArrive = sellers[dist.lorries[i].sellerID].GetTravelTime();
			std::cout << ("Truck  " + to_string(i + 1) + " is returning from shop " + to_string(dist.lorries[i].sellerID+1) + " and will arrive in " + to_string(dist.lorries[i].daysUntilArrive) + " days\n");
			dist.lorries[i].phase = RETURNING;
		}
		else if (dist.lorries[i].phase == RETURNING)
		{
			if (dist.lorries[i].daysUntilArrive > 1)
			{
				dist.lorries[i].daysUntilArrive -= 1;
				std::cout << ("Truck  " + to_string(i + 1) + " is returning from shop " + to_string(dist.lorries[i].sellerID+1) + " and will arrive in " + to_string(dist.lorries[i].daysUntilArrive) + " days\n");
			}
			else
			{
				dist.lorries[i].daysUntilArrive -= 1;
				std::cout << ("Truck  " + to_string(i + 1) + " has returned from shop " + to_string(dist.lorries[i].sellerID +1) + " and is waiting to be dispatched\n");
				dist.lorries[i].phase = IDLE;
			}
		}
	} 

}


void ProcessDay()
{
	//CheckLorries
	CheckLorries();
	//Sell
	for (int i = 0; i < numOfSellers; i++)
	{
		Sell(i);
	}
	
}



int main()
{
	srand(std::time(nullptr));

	ReadInFiles();
	SetUpFuzzyLogic();
	float daysToProcess = 365;
	
	//processDays 
	for (int i = 1; i <= daysToProcess; i++)
	{
		std::cout << ("Day " + to_string(i) + ":\n\n") ;
		ProcessDay();
		std::cout << ("\n");
	}

	float total = 0;
	for (int i = 0; i < numOfSellers; i++)
	{
		cout << ("\nSALES MISSED: " + to_string(salesMissed[i]) + "in shop " + to_string(i+1)+"\n");
		cout << ("\nSALES MISSED PER DAY: " + to_string(salesMissed[i] / daysToProcess) + " in shop " + to_string(i + 1) + "\n");
		total += salesMissed[i];
		std::cout << ("\n");
	}

	std::cout << ("------------------------------------------\n");

	cout << ("\nTOTAl SALES MISSED: " + to_string(total) + "\n");
	cout << ("\nTOTAL SALES MISSED PER DAY: " + to_string(total/daysToProcess) + "\n");
   
}




