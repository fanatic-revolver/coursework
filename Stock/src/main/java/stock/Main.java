//If there is something that is not so clear,please contact
//email:scyjm2@nottingham.edu.cn    tel:+86 13916493742    Jingyu Ma (John)
package main.java.stock;
import java.io.*;
import java.util.*;
import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;
import com.alibaba.fastjson.annotation.JSONField;
import org.apache.commons.io.FileUtils;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
//Using some packages from Alibaba fastjson
//For the ID part this code is  using UUID which is from International Organization for Standardization(ISO)


public class Main<pQueue>  {

    static PriorityQueue<Buyorder>PQ_BUY=new PriorityQueue<Buyorder>();    //initialize the PriorityQueue for Buy orders
    static PriorityQueue<Sellorder>PQ_SELL=new PriorityQueue<Sellorder>();         //initialize the PriorityQueue for Sell orders


    //main method is basically a dead loop,which you can do as many command as you want,all actions for CW is included
    public static void main(String[] args) {
        System.out.println("");
        System.out.println("----These are the commands----");
        System.out.println("r:read the JSON file");
        System.out.println("q/quit:quit the program");
        System.out.println("b/buy:insert a buy order");
        System.out.println("s/sell:insert a sell order");
        System.out.println("e:execute the order");
        System.out.println("qs:quit and save order to local");
        System.out.println("gb:generate some buy order");
        System.out.println("gs:generate some sell order");
        System.out.println("----These are the commands----");
        System.out.println("");
        boolean condition=true;     //this variable is mainly used to control the program to quit in a endless loop

    while(condition){
        System.out.print("Enter a command:");
        Scanner scan=new Scanner(System.in);     //use scan to scan input
        String action = scan.nextLine();         //put whatever you read in to action


        switch (action) {

            case "r":                                                                              //if the action is r,read from the json files in the resources folder
                String path = Main.class.getClassLoader().getResource("buy.json").getPath();//find the path for buy.json
                String s = FileUtil.ReadFile(path);                                                //put what you read in the file into string s
                JSONObject jobj = JSON.parseObject(s);                                             //use parseobject to parse string to alibaba.fastjson.JSONobject (similar to java object)
                JSONArray BUY_LIST = jobj.getJSONArray("BUY_LIST");                           //search the sting for a array called BUY_LIST
                for (int i = 0 ; i < BUY_LIST.size();i++){                                         //loop through the array to put every price and amount to the
                    JSONObject key1 = (JSONObject)BUY_LIST.get(i);
                    int PRICE = Integer.parseInt((String)key1.get("Price"));                          //put the price and amount into variable
                    int AMOUNT = Integer.parseInt((String)key1.get("Amount"));
                    String uuid = UUID.randomUUID().toString().replaceAll("-", "");//gemerate a UUID
                    Buyorder thisbuyorder = new Buyorder(PRICE, AMOUNT, uuid);                     //new a Buyorder object and insert it
                    InsertBuyOrder(thisbuyorder);
                }

                 path = Main.class.getClassLoader().getResource("sell.json").getPath();      //All these are simliar to the upper part
                 s = FileUtil.ReadFile(path);                                                       //You read from the file and insert the data into priorirty queue
                 jobj = JSON.parseObject(s);
                JSONArray SELL_LIST = jobj.getJSONArray("SELL_LIST");
                for (int i = 0 ; i < SELL_LIST.size();i++){
                    JSONObject key1 = (JSONObject)SELL_LIST.get(i);
                    int PRICE = Integer.parseInt((String)key1.get("Price"));
                    int AMOUNT = Integer.parseInt((String)key1.get("Amount"));
                    String uuid = UUID.randomUUID().toString().replaceAll("-", "");
                    Sellorder thissellorder = new Sellorder(PRICE, AMOUNT, uuid);
                    InsertSellOrder(thissellorder);
                }
                maxPrint(PQ_BUY);                        //Print out the max heap of PQ_Buy,you can see the bigger number is at the top
                System.out.println("");
                minPrint(PQ_SELL);                       //Print out min heap of PQ_Sell ,you can see smaller number at the top
                break;
            case"qs":                                //if the action is qs,save the priority queue to output_buy and output_sell
                 path = Main.class.getClassLoader().getResource("").getPath();
                List<Object> list = new ArrayList<>();       //intialize a list to store price and amount
                while(!PQ_BUY.isEmpty())                     //loop to get all values from PQ_BUY
                {
                    list.add(PQ_BUY.remove());
                }
                String jsonString1 = JSON.toJSONString(list);  //Convert the list to string of JSON format
                CreateFileUtil.createJsonFile(jsonString1, path, "output_buy");  //create a file at "path" with json string  we just read
                list = new ArrayList<>();
                while(!PQ_SELL.isEmpty())
                {
                    list.add(PQ_SELL.remove());
                }
                 jsonString1 = JSON.toJSONString(list);
                CreateFileUtil.createJsonFile(jsonString1, path, "output_sell");
                condition=false;
                break;
            case "b":                                                         //The following code is used to insert a buy order
                System.out.println("Please input Price");
                Scanner sc = new Scanner(System.in);
                int price = sc.nextInt();
                System.out.println("Please input Amount");
                int amount = sc.nextInt();
                String uuid = UUID.randomUUID().toString().replaceAll("-", "");
                Buyorder thisbuyorder = new Buyorder(price, amount, uuid);
                InsertBuyOrder(thisbuyorder);
                System.out.println("Success for insertion!");
                maxPrint(PQ_BUY);
                break;

            case ("s"):                                                //The following code is used to insert a sell order
                System.out.println("Please input Price");
                 sc = new Scanner(System.in);
                 price = sc.nextInt();
                System.out.println("Please input Amount");
                amount = sc.nextInt();
                uuid = UUID.randomUUID().toString().replaceAll("-", "");
                Sellorder thissellorder = new Sellorder(price,amount,uuid);
                InsertSellOrder(thissellorder);
                System.out.println("Success for insertion!");
                minPrint(PQ_SELL);
                break;

            case ("sell"):                                         //same as action s,insert a sell order
                System.out.println("Please input Price");
                sc = new Scanner(System.in);
                price = sc.nextInt();
                System.out.println("Please input Amount");
                amount = sc.nextInt();
                uuid = UUID.randomUUID().toString().replaceAll("-", "");
                thissellorder = new Sellorder(price,amount,uuid);
                InsertSellOrder(thissellorder);
                System.out.println("Success for insertion!");
                minPrint(PQ_SELL);
                break;

            case "e":                                                //execute the order
                ExecuteOrder(PQ_BUY,PQ_SELL);
                condition=false;
                break;

            case "gb":                                              //generate some buy orders
                    System.out.println("How much buy order list you want to generate?");
                    sc = new Scanner(System.in);
                    int quantity=sc.nextInt();
                    for(int i=1;i<=quantity;i++)
                    {
                        uuid = UUID.randomUUID().toString().replaceAll("-", "");
                        thisbuyorder = new Buyorder(80+(int) (Math.random() * 30)+(int)Math.round(Math.random()),(int)(1+Math.random()*10)*100,uuid);
                        InsertBuyOrder(thisbuyorder);
                    }
                    System.out.println("");
                    System.out.println("Buy order generation success!");
                    maxPrint(PQ_BUY);
                break;

            case "gs":                                   //generate some sell order
                System.out.println("How much sell order do you want to generate?");
                sc = new Scanner(System.in);
                 quantity=sc.nextInt();
                for(int i=1;i<=quantity;i++)
                {
                    uuid = UUID.randomUUID().toString().replaceAll("-", "");
                    thissellorder = new Sellorder(100+(int) ( Math.random()*30)+(int)Math.round(Math.random()),(int)(1+Math.random()*10)*100,uuid);
                    InsertSellOrder(thissellorder);
                }
                System.out.println("");
                System.out.println("Sell order generaton success!");
                minPrint(PQ_SELL);
                break;

            case "q":                                 //quit the program
                condition=false;
                break;

            case "quit":
                condition=false;                      //quit the program
                break;

            default:
            System.out.println("No such action!");   //indicated your input is wrong
            break;
        }
        }

    }

    //This methods print the PriorityQueue from a price ascending order
    public static void minPrint(PriorityQueue<Sellorder> pqSell) {
        PriorityQueue<Sellorder> tempQ=new PriorityQueue<Sellorder>();
        Iterator<Sellorder> iterator1 = pqSell.iterator();
        //iterator.hasNext()如果存在元素的话返回true
        while(iterator1.hasNext()) {
            //iterator.next()返回迭代的下一个元素
            tempQ.add(iterator1.next());
        }
        int count =1;
        while(!tempQ.isEmpty())
        {
            System.out.println("order"+count+"-->"+tempQ.poll());
            count++;
        }
    }

    //This methods print the PriorityQueue from a price descending order
    public static void maxPrint(PriorityQueue<Buyorder> pqBuy) {
        PriorityQueue<Buyorder> tempQ=new PriorityQueue<Buyorder>();
        Iterator<Buyorder> iterator1 = pqBuy.iterator();
        //iterator.hasNext()如果存在元素的话返回true
        while(iterator1.hasNext()) {
            //iterator.next()返回迭代的下一个元素
            tempQ.add(iterator1.next());
        }

        int count =1;
        while(!tempQ.isEmpty())
        {
            System.out.println("order"+count+"-->"+tempQ.remove());
            count++;
        }
    }

    //This method is about how to execute all orders in the order list
    public static void ExecuteOrder(PriorityQueue<Buyorder> pqBuy, PriorityQueue<Sellorder> pqSell) {
        boolean flag=true;
        System.out.println("");
        System.out.println("");
        System.out.println("");
        System.out.println("");
        System.out.println("");
        System.out.println("");
        System.out.println("Executed:");
        while(flag)
        {
            if(pqBuy.peek().getPrice()<pqSell.peek().getPrice())                 //compare the price first,if Buy list's biggest is smaller than the sell orders' smallest then quit loop
            {
                System.out.println("(This line is to indicate you in case nothing is executed)");
                flag=false;
                break;
            }
            if(pqBuy.peek().getPrice()>=pqSell.peek().getPrice())             //after compare,if the state is  we can execute some order,then we begin we begin to compare the amount,see which one has more
            {

               if(pqBuy.peek().getAmount()>pqSell.peek().getAmount())        //if we see that buy amount is larger,means buy order is partillly executed,execute both the make a new buy order and insert it
               {
                   int Buyamount=pqBuy.peek().getAmount();
                   String BuyID=pqBuy.peek().getID();
                   int BuyPrice=pqBuy.peek().getPrice();
                   int Sellamount=pqSell.peek().getAmount();
                   int remain_amount=Buyamount-Sellamount;
                   System.out.println(new Buyorder(pqBuy.peek().getPrice(),Sellamount,BuyID));
                   pqBuy.remove();
                   pqBuy.add(new Buyorder(BuyPrice,remain_amount,BuyID));
                   System.out.println(pqSell.remove());

               }
                else if(pqBuy.peek().getAmount()<pqSell.peek().getAmount())   //if we see that but amount is lesser,then the sell order is partially executed,execute both and make a new sell order with remain amount and insert it
                {
                    int Buyamount=pqBuy.peek().getAmount();
                    String SellID=pqSell.peek().getID();
                    int Sellamount=pqSell.peek().getAmount();
                    int SellPrice=pqSell.peek().getPrice();
                    int remain_amount=Sellamount-Buyamount;                 //the remain amount is SellAmount-BuyAmount
                    System.out.println(pqBuy.remove());                     //print the removed element
                    System.out.println(new Sellorder(pqSell.peek().getPrice(),Buyamount,SellID));
                    pqSell.remove();                                        //as the head element is executed we just remove it,delete it
                    pqSell.add(new Sellorder(SellPrice,remain_amount,SellID));

                }

               else if(pqBuy.peek().getAmount().equals(pqSell.peek().getAmount())) //if we see that they are equal then execute both
                {
                    System.out.println(pqBuy.poll());         //poll is used to Retrieves and removes the head of this queue, or returns null if this queue is empty
                    System.out.println(pqSell.poll());
                }

            }


        }
        System.out.println("Succeed in executing!!!");                   //Here just print all the remaining queue to let you see
        System.out.println("");
        System.out.println("Remaining Buy PriorityQueue:");
        maxPrint(PQ_BUY);
        System.out.println("");
        System.out.println("Remaining Sell PriorityQueue:");
        minPrint(PQ_SELL);

    }

    //This method is about how to insert a sell order
    public static void InsertSellOrder(Sellorder thissellorder) {
        PQ_SELL.add(thissellorder);
    }

    //This method is about how to insert a buy order
    public static void InsertBuyOrder(Buyorder thisbuyorder) {
        PQ_BUY.add(thisbuyorder);
    }

    

}
