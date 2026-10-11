#ifndef DL_TEMPLATE_GLOBALCONSTS_H
#define DL_TEMPLATE_GLOBALCONSTS_H

// Different macros for the three uses of static
// first is for global static variables, which have internal linkage (local to the translation unit)
// second is for class static variables, shared by all objects of that class
// third is for local function static variables, which are store din static memory
#define LOCAL static
#define SHARED static
#define STATIC static



#endif