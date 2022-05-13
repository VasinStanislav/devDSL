#ifndef EXPRASTNode_H
#define EXPRASTNode_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Token.h"

/*-base-*/ class ASTNode; 

typedef std::string                  string;
typedef const std::string &          ConstStrRef;
typedef std::pair<string, string>    StrPair;
typedef const StrPair                ConstStrPairRef;
typedef std::vector<string>          StrV;
typedef const StrV &                 ConstStrVRef;
typedef std::vector<ASTNode *>       NodeV;
typedef std::map<string, int>        PriorityMap;

/*----------------------------------------------BASE--------------------------------------------------*/

class ASTNode 
{
        friend bool isEqualWith(ASTNode &, ConstStrRef);
        friend void swap(ASTNode *, ASTNode *);

        ASTNode *parentPtr;
        Token    label;
        NodeV    children;
    protected:
        NodeV *getChildrenPtr();
    public:
        ASTNode();
        ASTNode(ASTNode *);
        ASTNode(Token, ASTNode *parentPtr=nullptr);
     /*-virtual-*/ ~ASTNode();

        void setParentPtr(ASTNode *);
        ASTNode * getParentPtr();

        void setLabel(Token);
        void setLabel(ConstStrRef, ConstStrRef, unsigned int);
        Token getLabel();

        size_t getChildrenAmount();

        void addChild(ASTNode *);

                ASTNode * getFirstChild();
                ASTNode * getLastChild();
        virtual ASTNode * getLastNode();
        void deleteFirstChild();
        void deleteLastChild();

        void showTree();
};

/*----------------------------------------ABSTRACT SYNTAX TREE----------------------------------------*/

class AST : public ASTNode
{
    public:
        AST(Token);
        ASTNode * getLastNode();
        void deleteLastNode();
};

/*------------------------------------------PREDENCY CONTROL------------------------------------------*/

class PredencyControl
{
        const static PriorityMap PRIORITY_MAP;
        static int curPriority;
    public:
        static void initPriority();
        static void setCurPriority(string);
        static void setCurPriority(int);
        static int  getCurPriority();
        static int  getPredency(string);
};

#endif