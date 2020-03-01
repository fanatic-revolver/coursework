package main.java.stock;

//This class is used to format the key-value pair like [{price:x,amount:y},{price:z,amount:a}]
public class OutputFormat {
    public Integer price;
    public Integer amount;

    public Integer getPrice() {
        return price;
    }

    public void setPrice(Integer Price) {
        this.price = Price;
    }

    public Integer getAmount() {
        return amount;
    }

    public void setAmount(Integer Amount) {
        this.amount = Amount;
    }

    public  OutputFormat(Integer Price, Integer Amount){
        this.price =Price;
        this.amount =Amount;
    }
}
