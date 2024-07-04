# XRE Lib


## Table of Contents


1. [Description](#description)
2. [Summary](#summary)
3. [Usage](#usage)
	1. [Minimal Example](#minimal-example)
	2. [More Examples](#more-examples)
4. [How does it work](#how-does-it-work)
	1. [Low level: States, context and components](#low-level-states-contexts-and-components)
	2. [Middle level: References](#middle-level-references)
	3. [High level: Hooks](#high-level-hooks)
		1. [Use state](#use-state)
		2. [Use effect](#use-effect)
	4. [Example](#example)
5. [License](#license)


## Description


This library is a memory state manager for iterative function invocations for the _C programming language_.
The library provide tools to keep memory among function calls, as well as automatize clean ups and updates.


## Summary


This project was born as an experiment to determine if it would be possible to bring the ideas of [React](https://react.dev/)'s hooks to _C_.


It turned out that yes, it is possible. At least partially.
As result this project turned it self into the _XRE library_.


## Usage


In my experience, I found this library especially useful when I develop:


- Loop-based applications, for example: games using [Raylib](https://www.raylib.com/).
- Application with multiple components which should keep their state through component switching.


But probably there are other use-cases where this library is also useful (let me know).


On _how_ to use it, for now, we will only provide examples and a the header files.


### Minimal example


```c
#include <stdio.h>
#include <xre.h>


void print_acc(XREContext *ctx, va_list args) {
    int partial_value = va_arg(args, int);

    XREStateInt * sum_state = xre_use_int(ctx, 0);

    int sum = xre_state_get_int(sum_state);

    sum += partial_value;
    printf("%d\n", sum);

    xre_state_set_int(sum_state, sum);
}


int main() {
    XREContext * ctx = xre_context_root_alloc();
    if (ctx == NULL) {
        return 1;
    }

    for (int value; scanf("%d", &value) != EOF; ) {
        xre_use_root(ctx, &print_acc, value);
    }

    xre_context_destroy(ctx);
    return 0;
}
```

This program reads a sequence of integers from `stdin` and provides the partial sum on the go.
Observe that the variable containing the global sum (`sum`) is defined _inside_ the function and keeps its value among different calls.


### More examples


You can find a couple of more complete examples under `/examples` folder.
They haven't been written carefully. Sorry about that. However they provide enough information to use all the features this library provides.


## How does it work


The API and its concepts can be categorized into three abstraction levels.


### Low level: States, contexts and components


A _context state_ or simply an _state_ is a piece of memory persistent among function calls which is able to clean up itself at _context_ destruction time. The object definition is private.


A _context_ is a sequence states. Because it is a sequence, _context states_ should be accessed in the same exact order (ie. sequentially) at every _component_ call.
Contexts relate among them hierachically in a tree structure. Concretely *contexts* can reference to child *contexts* by using labels.


A _component_ is the function where a _context_ is running from the beginning to its end. They have the exactly following signature:


```c
typedef void (*XREComponent)(struct XREContext *, va_list);
```


When a _component_ is invoked it always does throught `xre_use` (or derivates). That function will create or use the corresponding associated context. 
During the *component* execution is when we can sequentialy access to the *context's states* or invoke child *contexts*.

To access the *context's states* we use the low-level API, directly or indirectly.

We can invoke children *contexts* with `xre_use`, passing the current *context* and the child's label as arguments.



### Middle level: References


References are a next level of abstraction over the _context states_. Similarly to them, the function of _references_ is to point to any value, but they have two main differences with respect _context states_:


1. They control over the pointed data is more complete. Concretely, they are composed by:


  1.1. A constructor, so they can build the value on first component's call.
  1.2. A destructor, so they can clean up the memory when the associated context is destroyed.
  1.3. A setter, so we can modify the value.
  1.4. A comparator, so we can know if the value has changed during the component's call.


2. They definition is public, so we can extend the struct and use it as a base to create custom structures.


They are mapped one to one with _context states_, so effectively _references_ behave similar to _context states_ and have the same restrictions.
That means that the `xre_use_ref` functions in a _context_ should be always be called in the same exact order at every _component_ call.


Because the above two points, _references_ are the foundations to the high level elements.


### High level: Hooks


Because of the story of this library, we use the same terminology as [[React]] uses for their equivalent in this library.


A _hook_ is any function inside a _component_ that passes the associated _context_ as an argument.


Because _hooks_ __could__, directly or indirectly, access the _context's states_, they __must__ follow the _Rules of Hooks_:


1. Call _hooks_ at same order at each _component_ call.


2. Pass the __current__ _context_ as a parameter.


Observe that those rules are direct translations from React's [Rules of Hooks](https://react.dev/reference/rules/rules-of-hooks).


First rule is the translation of _"Only call hooks at the top level"_.
Second rule is the translation of _"Only call hooks from React functions"_.


Not to follow the _Rule of Hooks_ leads the program to an undefined behaviour, and probably to a `SEGFAULT`. Therefore, from now on we are always assuming every _hook_ follow the _Rules of Hooks_.


Then, to be a _hook_ is an interesting property, because any regular function containing one or more _hooks_ is a _hook_.


The converse, however, is not always true. There are _atomic hooks_ which internally use _references_ or event the low-level API to access the _context states_.


Also, according to that definition, when _components_ aren't called throught `xre_use` they become _hooks_.


As a final note, a convention used in this library is that _hooks_ and only _hooks_ are prefixed by `xre_use`. We recommend to keep that convention at the main application.


Summarizing we say that _hook_ are the high level bricks that can be composed or directly used to endow the application with the main functionality provided by the library.


We finish this section by explaining two _hooks_ built in this library ready to be used.


#### Use state


The _hook_ `xre_use_<type>(ctx, initial_value)` uses exactly one _context state_ through a _reference_ to store in memory a variable of type `<type>`. The returned value allow us to get, modify or detect changes on that value.
Allocation and free are automatically handled on context creation and destruction.


The `<type>` can be one of: `int`, `long int`, `size_t`, `char`, `void *` and `void const *`.


#### Use effect


The _hook_ `xre_use_effect(ctx, effect, dependencies, ...)` allows us to perform any side effect and effect's clean up when any _reference_ in the `dependencies` array changes.
It can be also configure to run the effect and the clean up either at each _component_ call or only at _context_ construction and destruction.


### Example


All the above concepts appear (directly or indirectly) in the following the partial example:


```c
void app(XREContext *ctx, va_list args) {
    (void) args;

    XREStateInt * fail_counter_state = xre_use_int(ctx, 0);
    XREStatePtr * command_hist_state = xre_use_ptr(ctx, NULL);
    XREStateString * command_state = xre_use_string(ctx, NULL);

    static XRERef const * cmd_hist_deps[] = {NULL};
    xre_use_effect(ctx, &init_command_hist, cmd_hist_deps, command_hist_state);

    char command[256];
    scanf("%s", command);
    xre_state_set_string(command_state, command);

    XRERef const * cmd_append_deps[] = {&command_state->ref, NULL};
    xre_use_effect(
        ctx,
        &append_command,
        cmd_append_deps,
        command_hist_state,
        command
    );

    if (strcmp(command, "back") == 0) {
        xre_state_set_int(fail_counter_state, 0);
        return;
    } else if (strcmp(command, "order") == 0) {
        xre_state_set_int(fail_counter_state, 0);
        xre_use(ctx, "order_screen", &order_screen);
    } else if (strcmp(command, "catalog") == 0) {
        xre_state_set_int(fail_counter_state, 0);
        xre_use(ctx, "catalog_screen", &catalog_screen);
    } else {
        int fail_counter = xre_state_get_int(fail_counter_state);
        xre_state_set_int(fail_counter_state, fail_counter + 1);
    }

    int fail_counter = xre_state_get_int(fail_counter_state);
    if (fail_counter >= 3) {
        print_help();
    }
}
```


## License


This project is under the MIT license.
