#COMMAND DISPATCH TABLE
.data
cmd_table:
    # char*, label to call
    .word cmdt_echo, con_echo
    .word cmdt_test, con_test
cmd_table_strings:
    cmdt_echo:
        .string "echo"
    cmdt_test:
        .string "test"