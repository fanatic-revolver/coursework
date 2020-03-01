package main.java.stock;
//This class is used to make the sell order Priority Queue,which overwrite the compare method
public class Sellorder implements Comparable <Sellorder>{
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

    public Sellorder(){}

    public Sellorder(Integer Price, Integer Amount, String ID){
        this.Price=Price;
        this.Amount=Amount;
        this.ID=ID;
    }

    @Override
    public String toString() {
        return "Sellorder{" +
                "Price=" + Price +
                ", Amount=" + Amount +
                ", ID=" + ID +
                '}';

    }

    public boolean equals(Sellorder other){
        return this.getID()==other.getID();
    }

    public int compareTo(Sellorder other) {
        if(this.equals(other))
            return 0;
        else if((getPrice()==other.getPrice())&&(getAmount()>other.getAmount()))   //We put the price with lower price first,then compare the amount
            return 1;
        else if(getPrice()>other.getPrice())
            return 1;
        else
            return -1;
    }
}
