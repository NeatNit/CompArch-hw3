/* 046267 Computer Architecture - Spring 21 - HW #3               */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include "dflow_calc.h"
#include <vector>

static const int NUM_REGS = 32;

struct Inst
{
    unsigned int latency;
    int depth;
    int dependency1;
    int dependency2;
};

class DFlowCalc
{
    unsigned int numOfInsts;
    int progDepth;
    std::vector<Inst> instructions;
public:
    DFlowCalc(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts)
    : numOfInsts(numOfInsts), progDepth(0), instructions(numOfInsts) {
        // this vector will store the last write to each register
        std::vector<int> register_writes(NUM_REGS, -1);

        // analyze the instructions in order of execution:
        InstInfo info;
        for (int i = 0; i < static_cast<int>(numOfInsts); ++i)
        {
            info = progTrace[i];
            Inst& result = instructions[i];

            // Store this instruction's latency
            result.latency = opsLatency[info.opcode];
            // also initialize other parameters
            result.depth = 0;
            result.dependency1 = -1;
            result.dependency2 = -1;

            // Find and store this instruction's dependencies
            int dep1i = register_writes[info.src1Idx];
            if (dep1i >= 0) {
                // dep1i wrote to source register, Read After Write
                Inst dep = instructions[dep1i];
                result.dependency1 = dep1i;
                result.depth = dep.latency + dep.depth; // definitely bigger than 0
            }

            int dep2i = register_writes[info.src2Idx];
            if (dep2i >= 0) {
                // dep2i wrote to source register, Read After Write
                Inst dep = instructions[dep2i];
                result.dependency2 = dep2i;
                int depth = dep.latency + dep.depth;
                if (depth > result.depth) {
                    result.depth = dep.latency + dep.depth; // definitely bigger than 0
                }
            }

            // update last write to register
            if (info.dstIdx >= 0) {
                register_writes[info.dstIdx] = i;
            }

            // update program depth
            if (result.depth + static_cast<int>(result.latency) > progDepth) {
                progDepth = result.depth + static_cast<int>(result.latency);
            }
        }
    }

    int getInstDepth(unsigned int i) {
        if (i >= numOfInsts) return -1;
        return instructions[i].depth;
    }

    int getInstDeps(unsigned int i, int & src1DepInst, int & src2DepInst) {
        if (i >= numOfInsts) return -1;
        src1DepInst = instructions[i].dependency1;
        src2DepInst = instructions[i].dependency2;
        return 0;
    }

    int getProgDepth() {
        return progDepth;
    }
};

ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    return static_cast<ProgCtx>(new DFlowCalc(opsLatency, progTrace, numOfInsts));
}

void freeProgCtx(ProgCtx ctx) {
    delete static_cast<DFlowCalc *>(ctx);
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    return static_cast<DFlowCalc *>(ctx)->getInstDepth(theInst);
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    return static_cast<DFlowCalc *>(ctx)->getInstDeps(theInst, *src1DepInst, *src2DepInst);
}

int getProgDepth(ProgCtx ctx) {
    return static_cast<DFlowCalc *>(ctx)->getProgDepth();
}


