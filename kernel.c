#include <stdint.h>
#include "interrupts/idt.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/commands/commands.h"
#include "drivers/basic/basic_storage.h"
#include "drivers/screen/screen.h" 
#include "drivers/fs/fat16.h"

void main(void) {
    const char *boot_msg = "BOOT SUCCESS - NYANMINI v1.0";
    
    // Wake up all our systems!
    screen_init();
    idt_init();
    storage_init(); 
    //fat16_init(); // <-- Let's tell this little kitty to take a nap for a second!
    set_keyboard_handler(command_handle_key);
    commands_init(); 
    __asm__ __volatile__("sti");
    enable_cursor(14, 15);

    // ... (the rest of the file is the same!) ...
    command_handle_key('\n');
    screen_clear();
    screen_print_string(boot_msg);
    print_prompt();
    for(;;) { __asm__ __volatile__("hlt"); }
}