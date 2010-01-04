;   ASXSoft Nuke - Operating System
;   Copyright (C) 2009  Patrick Pokatilo
;
;   This program is free software: you can redistribute it and/or modify
;   it under the terms of the GNU General Public License as published by
;   the Free Software Foundation, either version 3 of the License, or
;   (at your option) any later version.
;
;   This program is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;   GNU General Public License for more details.
;
;   You should have received a copy of the GNU General Public License
;   along with this program.  If not, see <http://www.gnu.org/licenses/>.

extern start_rocket_engine
global entry

section .multiboot

; Multiboot header
header dd 0x1BADB002
flags dd 0x00000007
checksum dd 0xE4524FF7
dd 0,0,0,0,0
mode dd 1
width dd 80
height dd 25
depth dd 0

section .text

; Deactivate interrupts, create stack and call main
entry:
cli
mov esp, 0x300000

push ebx
call start_rocket_engine

; Idle
idle:
cli
hlt
jmp idle
