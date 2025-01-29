const express=require("express"); //express modülü import edildi
const app=express(); //express modülü app değişkenine atandı
const mongoose= require("mongoose"); //mongoose modülü import edildi

app.use(express.json()); //json veri almak için kullanıldı
const PORT = 8082; //port numarası belirlendi
const bcrypt = require("bcryptjs"); 
const jwt = require('jsonwebtoken');

const mongoUrl="mongodb+srv://akslldilara:Dilara0234@cluster0.iyqaz.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0"; //veritabanı

const JWT_SECRET="hvdvay6ert72839289()aiyg8t87qt72393293883uhefiuh78ttq3ifi78272jdsds039[]]pou89ywe";  //jwt secret key
mongoose.  //veritabanı bağlantısı
connect(mongoUrl) //veritabanı url
.then(()=>{
    console.log("Veritabanı bağlandı");  //bağlantı başarılı olursa
})
.catch((e) => {
    console.log(e);  //bağlantı başarısız olursa
});

require('./UserDetails')
const User=mongoose.model("UserInfo")  //model oluşturuldu

app.get("/",(req,res)=>{
    res.send({status:"started"});  //başlangıç sayfası
});



app.post('/register',async(req, res)=>{ //kullanıcı kayıt işlemi
    const {full_name, email, password,  password_confirmation} = req.body; //kullanıcıdan alınan bilgiler
    console.log(req.body); //kullanıcıdan alınan bilgileri console da gösterir

    const oldUser = await User.findOne({ email: email }); //kullanıcı daha önce kayıt olmuş mu kontrolü

    if (oldUser) {
        return res.send({ data: "User already exists!!" }); //kullanıcı varsa hata mesajı
      }

      const encryptedPassword = await bcrypt.hash(password, 10);    //şifreleme işlemi

    try {
        await User.create({ //kullanıcı oluşturma
            full_name:full_name,
            email:email,
            password: encryptedPassword,
            password_confirmation: password_confirmation,
        });
        res.send({status: "ok", data:"User Created"}) //kullanıcı oluşturuldu
    } catch (error) {
        res.send({status: "error", data: error});
    }

});


app.post("/login-user",async(req,res)=>{
    const{email, password}=req.body;
    console.log(req.body);
    const oldUser=await User.findOne({email: email});

    if(!oldUser){
        return res.send({data:"Kullanıcı bulunamadı"}) //kullanıcı bulunamadı
    }
    
    if (await bcrypt.compare(password, oldUser.password)) {
        const token = jwt.sign({ email: oldUser.email }, JWT_SECRET);
        console.log(token);
        if (res.status(200)) {
          return res.send({
            status: "ok",
            data: token,
            userType: oldUser.userType,
          });
        } else {
          return res.send({ error: "error" });
        }
      }
    });

app.post("/userdata", async(req,res)=>{ //kullanıcı bilgileri
    const{token}=req.body;
    try {
        const user= jwt.verify(token, JWT_SECRET) //token doğrulama
        const useremail=user.email;
 
        User.findOne({email:useremail}).then(data=>{ //kullanıcı bilgileri
            return res.send({status: "Ok", data: data}); //kullanıcı bilgileri gösterilir
        });
    } catch (error) {
        return res.send({eror: "error"});
    }
});



app.listen(PORT,()=>{
   console.log("node.js başlatıldı");   //node.js başlatıldı
});