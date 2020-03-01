package main.java.stock;
//This method is used to create a buy order priority queue,which overwrite the compare method
public class Buyorder implements Comparable<Buyorder> {
    public Integer Price;
    public Integer Amount;
    public String ID;

    public Integer getPrice() {
        return Price;
    }

    public void setPrice(Integer Price) {
        this.Price = Price;
    }

    public Integer getAmount() {
        return Amount;
    }

    public void setAmount(Integer Amount) {
        this.Amount = Amount;
    }

    public void setID(String ID) {
        this.ID = ID;
    }

    public String getID() {
        return ID;
    }

    public void setAge(Integer Amount) {
        this.Amount= Amount;
    }

    public Buyorder(){}

    public Buyorder(Integer Price, Integer Amount, String ID){
        this.Price=Price;
        this.Amount=Amount;
        this.ID=ID;
    }

    @Override
    public String toString() {
        return "Buyorder{" +
                "Price=" + Price +
                ", Amount=" + Amount +
                ", ID=" + ID +
                '}';

    }

    public boolean equals(Buyorder other){
        return this.getID()==other.getID();
    }

    public int compareTo(Buyorder other) {
        if(this.equals(other))
            return 0;
        else if((getPrice()==other.getPrice())&&(getAmount()>other.getAmount()))  //We put the one with larger price front,with smaller amount first
            return 1;
        else if(getPrice()<other.getPrice())
            return 1;
        else
            return -1;
    }
}
