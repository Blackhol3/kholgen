#pragma once

class QTextStream;

void initStdout();
QTextStream& qStdout();
unsigned int divideCeil(unsigned int a, unsigned int b);
void preventSleepMode(bool shouldPreventSleepMode);
