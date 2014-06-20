using namespace std;
#include <iostream>
#include <algorithm>
#include "OrgL1.h"

/*
  Development mechanism based on context-sensitive L-systems.
  Each organism has a set of production rules of the form A<B>C -> DEF. The production rules are applied to the genome until no more rules can be applied.
  When development is finished, the result is the specification of the offspring's production rules and `solution' (These parts are separated by the `AA' symbol sequence.
  The production rule specification is read as follows. The first three symbols make the LHS. All following symbols up to but not including a punctuation symbol sequence
  make the RHS. The specification of the next rule starts after the punctuation.

  Flags:
  - noisy-development
  - half-development-process

 */

void OrgL1::printDevelopmentProcess(ofstream &stream)
{
    stream << "Production rules:" << endl;
    for(map<triple, vector<unsigned short> >::iterator it = productionRules.begin(); it != productionRules.end(); ++it)
    {
      stream <<  it->first.x << "<" << it->first.y << ">" << it->first.z << " -> ";
      for(vector<unsigned short>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
	stream << *it2 << " ";
      stream << endl;
    }
}

// Create a new developmentProcess, for an `orphan' cell
void OrgL1::initializeOrphanDevelopmentProcess(config &args)
{
  productionRules.clear();

  if (not (args.organismArgs.count("type") and args.organismArgs["type"] == "ga"))
  {
    unsigned int s = rand()%(initProductionRuleSize-10) + 10;
    for(int i=0; i<s; ++i)
      productionRuleSpecification.push_back(rand()%alphabetSize);
  
    buildProductionRules(args, productionRuleSpecification.begin(), productionRuleSpecification.end(), productionRules);
  }
}


// Done at the start of each reproduction cycle
void OrgL1::initializeDevelopmentProcess(config &args)
{
  OrgL1 *c = ((OrgL1*)child);

  workingGenome.at(0).clear();
  workingGenome.at(1).clear();

  // Copy child's genome to my working genome
  // If this flag is set, introduce non-heritable variation at the start of development
  workingGenome.at(workingGenomeFrame) = child->copyGenome(args);
  if (args.organismArgs.count("noisy-development"))
    mutateGenome(args, workingGenome.at(workingGenomeFrame));

}


void OrgL1::buildProductionRules(config &args, vector<unsigned short>::iterator it, vector<unsigned short>::iterator developmentProcessEnd, map<triple, vector<unsigned short> > &target)
{
  vector<unsigned short>::iterator developmentProcessStart = it;
  // Build child's production rules from the stuff before the punctuation mark
  while (developmentProcessEnd-it >= 3) // While there are enough symbols left to make the LHS of a production rule
  {
    triple LHS = {*it, *(it+1), *(it+2)};
    it += 3;
    vector<unsigned short>::iterator ruleEnd = developmentProcessEnd;
    unsigned int punctuationSize = 0;
    // todo search for earliest instance of any punctuation mark, instead of the earliest instance of the first one we check
    for (vector<vector<unsigned short> >::iterator p = rulePunctuationMarks.begin(); p != rulePunctuationMarks.end(); ++p)
    {
      vector<unsigned short>::iterator ruleEnd2 = search(it, developmentProcessEnd, p->begin(), p->end());
      if (ruleEnd2 - developmentProcessStart < ruleEnd - developmentProcessStart)
      {
	ruleEnd = ruleEnd2;
	punctuationSize = p->end() - p->begin();
      }
    }

    // TODO changed so that rules can only be so long
    if (ruleEnd - it > 10)
    {
      ruleEnd = it + 10;
      punctuationSize = 0;
    }

    vector<unsigned short> RHS;
    copy(it, ruleEnd, back_inserter(RHS));
    target[LHS] = RHS;
    it = ruleEnd + punctuationSize;
  }
}


// 1. Split the unpacked genome into two parts, one of which specifies the production rules of the next generation, and the other of which specifies whatever else the environment requires
// 2. Build rules from the first part
void OrgL1::interpret(config &args, vector<unsigned short> unpackedGenome)
{
  OrgL1 *c = (OrgL1*)child; 

  vector<unsigned short>::iterator developmentProcessEnd = unpackedGenome.end();
  unsigned int punctuationSize = 0;
  // If loopy mechanism, split the result into a specification of production rules and specification of the body
  if (type == LOOPY)
  {
    // Look for a part punctuation mark
    // todo search for earliest instance of any punctuation mark, instead of the earliest instance of the first one we check
    // todo I don't think this works if we've got more than one part punctuation mark. Look at the implementation of the rule punctuation searching below
    for (vector<vector<unsigned short> >::iterator p = partPunctuationMarks.begin(); p != partPunctuationMarks.end(); ++p)
    {
      vector<unsigned short>::iterator searchBegin;
      // If this flag is set, at least half of the unpacked genome specifies a developmentProcess, so we only look for the punctuation mark in the second half
      if (args.organismArgs.count("half-development-process"))
	searchBegin = unpackedGenome.begin() + unpackedGenome.size()/2;
      else
	searchBegin = unpackedGenome.begin();
      developmentProcessEnd = search(searchBegin, unpackedGenome.end(), p->begin(), p->end());
      if (developmentProcessEnd != unpackedGenome.end())
	punctuationSize = p->size();
    }
  }
  else // If unloopy mechanism, the whole unpacked genome specifies the body
    developmentProcessEnd = unpackedGenome.begin();

  // Copy the stuff after the punctuation mark to the child's body specification
  c->somaSpecification.clear();
  copy(developmentProcessEnd+punctuationSize, unpackedGenome.end(), back_inserter(c->somaSpecification));
  
  // If loopy, build the production rules using the stuff before the punctuation mark
  if (type == LOOPY)
    buildProductionRules(args, unpackedGenome.begin(), developmentProcessEnd, c->productionRules);
  else if (type == UNLOOPY) // else copy our production rules specification to the child, mutated
  {
    copy(productionRuleSpecification.begin(), productionRuleSpecification.end(), back_inserter(c->productionRuleSpecification));

    // Mutate the production rules  TODO decision. How to mutate, min/max size
    if (rand()%2==0 and c->productionRuleSpecification.size() < initProductionRuleSize*10)
    {
      if (c->productionRuleSpecification.size() == 0)
	c->productionRuleSpecification.push_back(rand()%alphabetSize);
      else
	c->productionRuleSpecification.insert(c->productionRuleSpecification.begin()+rand()%(c->productionRuleSpecification.size()), rand()%alphabetSize);
    }
    // Delete with probability 0.5
    if (rand()%2==0 and c->productionRuleSpecification.size() > 0)
      c->productionRuleSpecification.erase(c->productionRuleSpecification.begin()+rand()%(c->productionRuleSpecification.size()));
    // Change with probability 0.5
    if (rand()%2==0 and c->productionRuleSpecification.size() > 0)
      c->productionRuleSpecification.at(rand()%(c->productionRuleSpecification.size())) = rand()%alphabetSize;

    c->buildProductionRules(args, c->productionRuleSpecification.begin(), c->productionRuleSpecification.end(), c->productionRules);
  }


  // Debugging
  if(args.organismArgs.count("debug"))
  {
    cout << "Final working genome:" << endl;
    for(vector<unsigned short>::iterator it=workingGenome.at(workingGenomeFrame).begin(); it!=workingGenome.at(workingGenomeFrame).end(); ++it)
      cout << *it << " ";
    cout << endl << endl;

    cout << id << " produced child " << c->id << endl;
  
    if (c->somaSpecification.size() > 0)
    {
      cout << "Child's body specification:" << endl;
      for(vector<unsigned short>::iterator it=c->somaSpecification.begin(); it!=c->somaSpecification.end(); ++it)
	cout << *it << " ";
      cout << endl;
    }

    cout << "Child's production rule specification:" << endl;
    if (type == LOOPY)
    {
      for(vector<unsigned short>::iterator it=unpackedGenome.begin(); it!=developmentProcessEnd; ++it)
	cout << *it << " ";
    }
    else
    {
      for(vector<unsigned short>::iterator it=c->productionRuleSpecification.begin(); it!=c->productionRuleSpecification.end(); ++it)
	cout << *it << " ";      
    }
    cout << endl;

    cout << "Child's production rules:" << endl;
    for(map<triple, vector<unsigned short> >::iterator it = c->productionRules.begin(); it != c->productionRules.end(); ++it)
    {
      cout <<  it->first.x << "<" << it->first.y << ">" << it->first.z << " -> ";
      for(vector<unsigned short>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
	cout << *it2 << " ";
      cout << endl;
    }
  } // End of debugging

}



// Iteratively apply the rewrite rules (developmentProcess) to the genome
void OrgL1::updateDevelopment(config &args)
{
  OrgL1 *c = (OrgL1*)child;

  // Debugging
  if(args.organismArgs.count("debug"))
  {
    cout << "I am " << id << " producing child " << c->id << endl;
    cout << "Genome:" << endl;
    for(vector<unsigned short>::iterator it=genome.begin(); it!=genome.end(); ++it)
      cout << *it << " ";
    cout << endl << "Child's genome:" << endl;
    for(vector<unsigned short>::iterator it=c->genome.begin(); it!=c->genome.end(); ++it)
      cout << *it << " ";
    cout << endl << "Production rule specification:" << endl;
    for(vector<unsigned short>::iterator it = productionRuleSpecification.begin(); it != productionRuleSpecification.end(); ++it)
      cout << *it << " ";
    cout << endl << "Production rules:" << endl;
    for(map<triple, vector<unsigned short> >::iterator it = productionRules.begin(); it != productionRules.end(); ++it)
    {
      cout <<  it->first.x << "<" << it->first.y << ">" << it->first.z << " -> ";
      for(vector<unsigned short>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
	cout << *it2 << " ";
      cout << endl;
    }
    cout << "Working genome:" << endl;
    for(vector<unsigned short>::iterator it=workingGenome.at(workingGenomeFrame).begin(); it!=workingGenome.at(workingGenomeFrame).end(); ++it)
      cout << *it << " ";
    cout << endl << endl;
  } // End of debugging

  changedOnLastPass = false;
  workingGenome.at(1-workingGenomeFrame).clear();

  for (vector<unsigned short>::iterator it=workingGenome.at(workingGenomeFrame).begin(); it!=workingGenome.at(workingGenomeFrame).end(); ++it)
  {

    // if we're over the working genome limit, stop unpacking
    if (workingGenome.at(1-workingGenomeFrame).size() > maxWorkingGenomeSize)
    {
      workingGenome.at(1-workingGenomeFrame).erase(workingGenome.at(1-workingGenomeFrame).begin()+maxWorkingGenomeSize, workingGenome.at(1-workingGenomeFrame).end());
      changedOnLastPass = false;
      break;
    }

    // if there isn't a matching production rule, just copy the symbol
    // otherwise, copy the matched genome

    // If the symbol doesn't have a left or right neighbour
    if (it-workingGenome.at(workingGenomeFrame).begin() == 0 or workingGenome.at(workingGenomeFrame).end()-it == 1)
    {
      workingGenome.at(1-workingGenomeFrame).push_back(*it);
    }
    else
    {
      triple t = {*(it-1),*it,*(it+1)};
      map<triple, vector<unsigned short> >::iterator productionRule = productionRules.end();

      // Treat a '3' on the left or right as a wildcard symbol
      if (args.organismArgs.count("wildcard"))
      {
	if (t.x == 3)
	{
	  triple t2 = t;
	  for (int i=0; i<alphabetSize; ++i)
	  {
	    t2.x = i;
	    productionRule = productionRules.find(t2);
	    if (productionRule != productionRules.end())
	      break;
	  }
	}
	if (t.z == 3 and productionRule == productionRules.end())
	{
	  triple t2 = t;
	  for (int i=0; i<alphabetSize; ++i)
	  {
	    t2.z = i;
	    productionRule = productionRules.find(t2);
	    if (productionRule != productionRules.end())
	      break;
	  }
	}
      }

      if (productionRule == productionRules.end())
	productionRule = productionRules.find(t);

      // if there is no such production rule
      if (productionRule == productionRules.end())
      {
	workingGenome.at(1-workingGenomeFrame).push_back(*it);
      }
      else
      {
	changedOnLastPass = true;
	// Insert from the production rule
	copy(productionRule->second.begin(), productionRule->second.end(), back_inserter(workingGenome.at(1-workingGenomeFrame)));	

	// Debugging
	if(args.organismArgs.count("debug"))
	{
	cout << "Replacing " << *it << " with ";
	for(vector<unsigned short>::iterator it2=productionRule->second.begin(); it2!=productionRule->second.end(); ++it2)
	  cout << *it2 << " ";
	cout << endl;
	} // End debugging
      }
    }
  }

  // Switch the frames
  workingGenomeFrame = 1-workingGenomeFrame;

  if(args.organismArgs.count("debug"))
  {
    cout << "Working genome after one pass:" << endl;
    for(vector<unsigned short>::iterator it=workingGenome.at(workingGenomeFrame).begin(); it!=workingGenome.at(workingGenomeFrame).end(); ++it)
      cout << *it << " ";
    cout << endl << endl;
  }

  if (not changedOnLastPass) 
  {
    interpret(args, workingGenome.at(workingGenomeFrame));
    state=REPRODUCED;
  }

}

OrgL1::OrgL1(config &args, globalVars &global) : BaseOrganism(args, global)
{
  type = LOOPY;
  if ( args.organismArgs.count("type") )
  {
    if (args.organismArgs["type"] == "ga")
      type = GA;
    else if (args.organismArgs["type"] == "unloopy")
      type = UNLOOPY;
  }

  initProductionRuleSize = initialGenomeSize * 5;
  maxWorkingGenomeSize  = maxGenomeSize * 10;

  workingGenomeFrame = 0;
  vector<unsigned short> workingGenome1;
  vector<unsigned short> workingGenome2;
  workingGenome.push_back(workingGenome1);
  workingGenome.push_back(workingGenome2);

  partPunctuationMarks.push_back({1,1});
  rulePunctuationMarks.push_back({0,0});
  rulePunctuationMarks.push_back({2,2});
  rulePunctuationMarks.push_back({3,3});
  rulePunctuationMarks.push_back({0,2});
  rulePunctuationMarks.push_back({0,3});
  rulePunctuationMarks.push_back({2,0});
  rulePunctuationMarks.push_back({2,3});
  rulePunctuationMarks.push_back({3,0});
  rulePunctuationMarks.push_back({3,2});
}

OrgL1::~OrgL1()
{
}
