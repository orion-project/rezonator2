#include "Perf.h"

#ifdef Z_PERF_ENABLED

#include <QDebug>
#include <QElapsedTimer>
#include <stack>

namespace Z::Perf {

std::optional<QElapsedTimer> __timer;

struct Block
{
    int count = 0;
    qint64 duration = 0;
    qint64 start;

    Block() {}
    Block(const Block &other) = delete;
    Block(const Block &&other) = delete;
};

typedef std::pair<const char*, const char*> BlockKey;
typedef std::map<BlockKey, Block> Blocks;
Q_GLOBAL_STATIC(Blocks, __blocks);


struct StackItem
{
    const char *id;
    Block *block;
};

typedef std::stack<StackItem> Stack;
Q_GLOBAL_STATIC(Stack, __stack);

void reset()
{
    __timer.reset();
    __blocks->clear();
    while (!__stack->empty())
        __stack->pop();

}

void begin(const char *id)
{
    if (__stack->empty()) {
        __timer = QElapsedTimer();
        __timer->start();

        Block *b = &(*__blocks)[{id, nullptr}];
        b->count++;
        b->start = __timer->elapsed();
        __stack->push({id, b});
    } else {
        auto &top = __stack->top();
        Block *b = &(*__blocks)[{id, top.id}];
        b->count++;
        b->start = __timer->elapsed();
        __stack->push({id, b});
    }
}

void end()
{
    if (__stack->empty())
        return;

    Block *b = __stack->top().block;
    b->duration += __timer->elapsed() - b->start;

    __stack->pop();
    if (__stack->empty())
        __timer.reset();
}

void printBranch(const char *id, int level)
{
    for (auto it = __blocks->cbegin(); it != __blocks->cend(); it++)
        if (it->first.second == id) {
            qDebug() << QString(level*4, ' ') +
                QStringLiteral("%1: %2_ms [%3]")
                    .arg(it->first.first)
                    .arg(it->second.duration)
                    .arg(it->second.count);
            printBranch(it->first.first, level+1);
        }
}

void print()
{
    printBranch(nullptr, 0);
}

} // namespace Z::Perf

#endif
