instructions = [
        ("add", 0x0000), ("sub", 0x0001), ("mult", 0x0002), ("div", 0x0003), ("mod", 0x0004),
        ("and", 0x0005), ("or", 0x0006), ("xor", 0x0007), ("not", 0x0008), ("nand", 0x0009),
        ("nor", 0x000A), ("neg", 0x000B), ("lsh", 0x000C), ("rsh", 0x000D), ("rol", 0x000E),
        ("ror", 0x000F), ("eq", 0x0010), ("ne", 0x0011), ("lt", 0x0012), ("le", 0x0013),
        ("gt", 0x0014), ("ge", 0x0015), ("ult", 0x0016), ("ule", 0x0017), ("ugt", 0x0018),
        ("uge", 0x0019), ("z", 0x001A), ("nz", 0x001B), ("c", 0x001C), ("nc", 0x001D),
        ("n", 0x001E), ("nn", 0x001F), ("mov", 0x0020), ("lw", 0x0021), ("lb", 0x0022),
        ("comp", 0x0023), ("lea", 0x0024), ("push", 0x0025), ("pop", 0x0026), ("clr", 0x0027),
        ("inc", 0x0028), ("dec", 0x0029), ("memcpy", 0x002A), ("sw", 0x002B), ("sb", 0x002C),
        ("lbrom", 0x002D), ("lwrom", 0x002E), ("romcpy", 0x002F), ("mults", 0x0030), ("divs", 0x0031),
        ("mods", 0x0032), ("mult_fpt", 0x0033), ("div_fpt", 0x0034), ("mod_fpt", 0x0034),
        ("call", 0x0040), ("ret", 0x0041), ("jmp", 0x0042), ("jcond_z", 0x0043),
        ("jcond_nz", 0x0044), ("jcond_neg", 0x0045), ("jcond_nneg", 0x0046), ("jcond_pos", 0x0047),
        ("jcond_npos", 0x0048), ("nop", 0x0049), ("hlt", 0x004A), ("jal", 0x004B), ("retl", 0x004C),
    ]

def asm_mnemonic_set_generator():

    imm_suffixes = ["", "i", "i1", "i2"]

    print("const std::unordered_set<std::string> validOpcodeMnemonics = {")
    for base, _ in instructions:
        for suffix in imm_suffixes:
            print(f'    "{base}{suffix}",')
    print("}")

if __name__ == "__main__":
    asm_mnemonic_set_generator()
    input()
