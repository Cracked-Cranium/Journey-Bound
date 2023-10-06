// Gets the amounf of a Item from a item
char GetItemAmount(Item item)
{
    return (item.data >> 9); // Returns the item count (0-127)
}

// Gets the Item id from a item
short GetItemId(Item item)
{
    return (item.data & 0b111111111); // Returns the ID (0-511)
}

// Create an item from ID and amount
Item CreateItem(unsigned short id, unsigned char amount)
{
    return (Item){.data = (amount << 9) + id};
}