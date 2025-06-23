#COMMAND DISPATCH TABLE
.data
cmd_table:
    # char*, label to call
    .word cmd_t_str_0, 1 #some label
    .word cmd_t_str_1, 1 #some label
cmd_table_strings:
    cmd_t_str_0:
        .string "Hi."
    cmd_t_str_1:
        .string "Hello!"

