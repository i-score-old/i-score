#ifndef CSP_TYPES_H
#define CSP_TYPES_H

/*!
 * \file CSPTypes.h
 * \author Raphael Marczak (LaBRI)
 * \date 2008-2009
 */

#include <string>

#define NO_BOUND -1

#define NO_ID 0
#define ROOT_BOX_ID 1

#define NO_MAX_MODIFICATION 0

#define BEGIN_CONTROL_POINT_INDEX 1
#define END_CONTROL_POINT_INDEX 2

/// Allen relations types
enum AllenType { ALLEN_BEFORE, ALLEN_AFTER, ALLEN_EQUALS, ALLEN_MEETS, ALLEN_OVERLAPS, ALLEN_DURING, ALLEN_STARTS, ALLEN_FINISHES, ALLEN_NONE };

enum TemporalRelationType { ANTPOST_ANTERIORITY, ANTPOST_POSTERIORITY };

enum TriggerType { TRIGGER_DEFAULT, TRIGGER_START_TEMPO_CHANGE, TRIGGER_END_TEMPO_CHANGE};


const std::string ALLEN_BEFORE_STR = "before";
const std::string ALLEN_AFTER_STR = "after";
const std::string ALLEN_EQUALS_STR = "equals";
const std::string ALLEN_MEETS_STR = "meets";
const std::string ALLEN_OVERLAPS_STR = "overlaps";
const std::string ALLEN_DURING_STR = "during";
const std::string ALLEN_STARTS_STR = "starts";
const std::string ALLEN_FINISHES_STR = "finishes";

const std::string ANTPOST_ANTERIORITY_STR = "anteriority";
const std::string ANTPOST_POSTERIORITY_STR = "posteriority";


const unsigned int BOX_NOT_CREATED = NO_ID;
const unsigned int RELATION_NOT_CREATED = NO_ID;
const unsigned int NO_MOTHER = NO_ID;



/// Different types of variables linked to the boxes
enum VariableType { BEGIN_VAR_TYPE = 1, LENGTH_VAR_TYPE = 100, INTERVAL_VAR_TYPE = 50, VPOS_VAR_TYPE = 1, VSIZE_VAR_TYPE = 100 };

/// The available binary relations types
enum BinaryRelationType { EQ_RELATION = 0, NQ_RELATION = 1, LQ_RELATION = 2, LE_RELATION = 3, GQ_RELATION = 4, GR_RELATION = 5 };

/// The three temporal relations in Boxes
enum RelationType {ALLEN = 0, ANTPOST = 1, INTERVAL = 2, BOUNDING = 3};

#endif
