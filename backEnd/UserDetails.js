const mongoose = require("mongoose");

const UserDetailSchema = new mongoose.Schema({ //kullanıcı bilgileri
    full_name: String, //kullanıcı
    email: {type:String, unique:true}, //kullanıcı maili
    password: String, //kullanıcı şifresi
},{
    collection:"UserInfo" //kullanıcı bilgileri UserInfo adlı collection da tutulacak
});
mongoose.model("UserInfo", UserDetailSchema )   //model oluşturuldu