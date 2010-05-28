/*
 *   ASXSoft Nuke - Operating System
 *   Copyright (C) 2009  Patrick Pokatilo
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */     

#ifndef _ISR_H_
#define _ISR_H_

#include <stdint.h>
#include <task.h>

extern "C" void exception_stub_0();
extern "C" void exception_stub_1();
extern "C" void exception_stub_2();
extern "C" void exception_stub_3();
extern "C" void exception_stub_4();
extern "C" void exception_stub_5();
extern "C" void exception_stub_6();
extern "C" void exception_stub_7();
extern "C" void exception_stub_8();
extern "C" void exception_stub_10();
extern "C" void exception_stub_11();
extern "C" void exception_stub_12();
extern "C" void exception_stub_13();
extern "C" void exception_stub_14();
extern "C" void exception_stub_16();
extern "C" void exception_stub_17();
extern "C" void exception_stub_18();
extern "C" void exception_stub_19();

extern "C" void irq_stub_0();
extern "C" void irq_stub_1();
extern "C" void irq_stub_2();
extern "C" void irq_stub_3();
extern "C" void irq_stub_4();
extern "C" void irq_stub_5();
extern "C" void irq_stub_6();
extern "C" void irq_stub_7();
extern "C" void irq_stub_8();
extern "C" void irq_stub_9();
extern "C" void irq_stub_10();
extern "C" void irq_stub_11();
extern "C" void irq_stub_12();
extern "C" void irq_stub_13();
extern "C" void irq_stub_14();
extern "C" void irq_stub_15();

extern "C" void isr_null();
extern "C" void isr_system();

extern "C" void dump_info(TaskState *task);
extern "C" void handle_interrupt(TaskState *task);

#endif
